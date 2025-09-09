#include "EclipseCache.hpp"
#include "EclipseCompiler.hpp"
#include "EclipseStateManager.hpp"
#include "EclipseScriptLoader.hpp"
#include "EclipseIncludes.hpp"
#include "EclipseConfig.hpp"
#include "EclipseLogger.hpp"
#include <chrono>

/**
 *
 */
EclipseStateManager& EclipseStateManager::GetInstance()
{
    static EclipseStateManager instance;
    return instance;
}

/**
 *
 */
EclipseSolState* EclipseStateManager::GetStateForMap(int32 mapId)
{
    auto [it, inserted] = _states.try_emplace(mapId, nullptr);
    if(!inserted)
        return it->second.get();

    auto engine = std::make_unique<EclipseSolState>();
    ECLIPSE_LOG_DEBUG("Creating new Lua state for map " + std::to_string(mapId));

    if(engine->Initialize(mapId))
    {
        EclipseSolState* enginePtr = engine.get();
        it->second = std::move(engine);
        return enginePtr;
    }

    _states.erase(it);
    return nullptr;
}

/**
 *
 */
EclipseSolState* EclipseStateManager::GetGlobalState()
{
    return GetStateForMap(-1);
}

static bool ScriptPathComparator(const LuaScript& first, const LuaScript& second)
{
    return first.filepath < second.filepath;
}

bool EclipseStateManager::RunFromCache(sol::state& solState, const std::string& filePath, uint32& compiledCount, uint32& cachedCount)
{
    auto& cache = EclipseCache::GetInstance();

    if (!cache.IsInCache(filePath))
        return false;

    std::optional<sol::bytecode> bytecode = cache.GetBytecode(filePath);
    if (cache.IsScriptModified(filePath))
    {
        bytecode = EclipseCompiler::CompileLuaToByteCode(solState, filePath);
        if (bytecode.has_value())
        {
            cache.StoreByteCode(filePath, bytecode.value(), true);
            compiledCount++;
        }
    }

    if (!bytecode.has_value())
        return false;

    auto result = solState.safe_script(bytecode->as_string_view());
    if (!result.valid())
    {
        sol::error err = result;
        ECLIPSE_LOG_ERROR("[Eclipse]: Error executing cached `{}`: {}", filePath, err.what());
        return false;
    }

    cachedCount++;
    return true;
}

bool EclipseStateManager::RunFromFile(sol::state& solState, const std::string& filePath)
{
    sol::load_result loaded_script = solState.load_file(filePath);
    if (!loaded_script.valid())
    {
        sol::error err = loaded_script;
        ECLIPSE_LOG_ERROR("[Eclipse]: Error loading `{}`: {}", filePath, err.what());
        return false;
    }

    sol::protected_function_result result = loaded_script();
    if (!result.valid())
    {
        sol::error err = result;
        ECLIPSE_LOG_ERROR("[Eclipse]: Error executing `{}`: {}", filePath, err.what());
        return false;
    }

    return true;
}

void EclipseStateManager::RunAllScripts()
{
    auto startTime = std::chrono::high_resolution_clock::now();
    bool cacheEnabled = EclipseConfig::GetInstance().IsByteCodeCacheEnabled();

    uint32 count = 0;
    uint32 compiledCount = 0;
    uint32 cachedCount = 0;
    uint32 precompiledCount = 0;

    EclipseScriptLoader::ScriptList scripts;
    EclipseScriptLoader::ScriptList lua_scripts = EclipseScriptLoader::GetLuaScripts();
    EclipseScriptLoader::ScriptList lua_extensions = EclipseScriptLoader::GetLuaExtensions();

    lua_scripts.sort(ScriptPathComparator);
    lua_extensions.sort(ScriptPathComparator);

    scripts.insert(scripts.end(), lua_extensions.begin(), lua_extensions.end());
    scripts.insert(scripts.end(), lua_scripts.begin(), lua_scripts.end());

    std::unordered_map<std::string, std::string> loaded;
    loaded.reserve(scripts.size());

    EclipseSolState* globalState = GetGlobalState();
    if (!globalState->IsInitialized())
        return;

    sol::state& solState = globalState->GetState();

    for (const auto& script : scripts)
    {
        if (loaded.find(script.filename) != loaded.end())
        {
            ECLIPSE_LOG_ERROR("[Eclipse]: Error loading `{}`. File with same name already loaded from `{}`, rename either file", script.filepath, loaded[script.filename]);
            continue;
        }

        if (script.fileext != ".lua" && script.fileext != ".ext")
            continue;

        loaded[script.filename] = script.filepath;
        bool success = false;

        if (cacheEnabled && RunFromCache(solState, script.filepath, compiledCount, cachedCount))
        {
            ECLIPSE_LOG_DEBUG("[Eclipse]: Successfully loaded from cache `{}`", script.filepath);
            success = true;
        }
        else if (RunFromFile(solState, script.filepath))
        {
            ECLIPSE_LOG_DEBUG("[Eclipse]: Successfully loaded `{}`", script.filepath);
            success = true;
        }

        if (success)
            ++count;
    }

    std::string details = "";
    if (cacheEnabled && (compiledCount > 0 || cachedCount > 0 || precompiledCount > 0))
    {
        details = fmt::format("({} compiled, {} cached, {} pre-compiled)", compiledCount, cachedCount, precompiledCount);
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    ECLIPSE_LOG_INFO("[Eclipse]: Executed {} Lua scripts in {} µs {}", count, static_cast<uint32>(duration), details);
}