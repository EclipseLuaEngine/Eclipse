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

    std::unordered_map<std::string, std::string> loaded; // filename, path


    EclipseSolState* globalState = GetGlobalState();
    if(!globalState->IsInitialized())
        return;

    sol::state& solState = globalState->GetState();
    sol::table package = solState["package"];
    sol::table modules = package["loaded"];

    for (const auto& script : scripts)
    {
        if (loaded.find(script.filename) != loaded.end())
        {
            ECLIPSE_LOG_ERROR("[Eclipse]: Error loading `{}`. File with same name already loaded from `{}`, rename either file", script.filepath, loaded[script.filename]);
            continue;
        }

        sol::load_result loaded_script;

        if (script.fileext == ".lua" || script.fileext == ".ext")
        {
            loaded_script = solState.load_file(script.filepath);
            if (!loaded_script.valid())
            {
                sol::error err = loaded_script;
                ECLIPSE_LOG_ERROR("[Eclipse]: Error loading `{}`: {}", script.filepath, err.what());
                continue;
            }

            sol::protected_function_result result = loaded_script();

            if (!result.valid())
            {
                sol::error err = result;
                ECLIPSE_LOG_ERROR("[Eclipse]: Error executing `{}`: {}", script.filepath, err.what());
                continue;
            }

            sol::object script_result = result.get<sol::object>();
            if (script_result == sol::lua_nil || (script_result.is<bool>() && !script_result.as<bool>()))
            {
                modules[script.filename] = true;
            }
            else
            {
                modules[script.filename] = script_result;
            }

            ECLIPSE_LOG_INFO("[Eclipse]: Successfully loaded `{}`", script.filepath);
            ++count;
        }
    }
}