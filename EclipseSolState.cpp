#include "EclipseIncludes.hpp"
#include "EclipseScriptLoader.hpp"
#include "EclipseLogger.hpp"
#include "EclipseSolState.hpp"
#include "EclipseCache.hpp"

EclipseSolState::EclipseSolState(Map* map) :
_isInitialized(false),
_solState(nullptr),
_map(map)
{
    Initialize();
    if(EclipseCache::GetInstance().GetCacheState() == SCRIPT_CACHE_READY)
        RunScripts();
}

/**
 *
 */
bool EclipseSolState::IsInitialized() const
{
    return _isInitialized;
}

/**
 *
 */
bool EclipseSolState::Initialize()
{
    if(IsInitialized())
        return true;

    try
    {
        _solState = sol::state();
        _solState.open_libraries(
            sol::lib::base,
            sol::lib::package,
            sol::lib::coroutine,
            sol::lib::string,
            sol::lib::os,
            sol::lib::math,
            sol::lib::table,
            sol::lib::debug,
            sol::lib::bit32,
            sol::lib::io,
            sol::lib::utf8
#ifdef SOL_LUAJIT
            , sol::lib::jit
#endif
        );

        _solState["package"]["path"] = EclipseScriptLoader::GetLuaRequirePath();
        _solState["package"]["cpath"] = EclipseScriptLoader::GetLuaRequireCPath();
        _solState.add_package_loader([this](const std::string& moduleName) -> sol::object {
            auto& cache = EclipseCache::GetInstance();
            
            const auto& extensionsMap = EclipseScriptLoader::GetLuaExtensionsMap();
            auto extIt = extensionsMap.find(moduleName);
            if (extIt != extensionsMap.end())
            {
                const LuaScript& script = extIt->second;
                std::optional<sol::bytecode> byteCode = cache.GetBytecode(script.filePath);
                if(byteCode.has_value())
                {
                    sol::load_result result = _solState.load(byteCode.value().as_string_view());
                    if(result.valid()) return result;
                }
            }
            
            const auto& scriptsMap = EclipseScriptLoader::GetLuaScriptsMap();
            auto scriptIt = scriptsMap.find(moduleName);
            if (scriptIt != scriptsMap.end())
            {
                const LuaScript& script = scriptIt->second;
                std::optional<sol::bytecode> byteCode = cache.GetBytecode(script.filePath);
                if(byteCode.has_value())
                {
                    sol::load_result result = _solState.load(byteCode.value().as_string_view());
                    if(result.valid()) return result;
                }
            }
            
            return sol::make_object(_solState, sol::lua_nil);
        });

        _isInitialized = true;

        ECLIPSE_LOG_DEBUG("[Eclipse]: Sol state initialized successfully");
        return true;
    }
    catch(const std::exception& e)
    {
        ECLIPSE_LOG_ERROR("[Eclipse]: Error during creation of sol state: " + static_cast<std::string>(e.what()));
    }

    return false;
}

/**
 *
 */
void EclipseSolState::RunScripts()
{
    if (!IsInitialized())
        return;

    sol::state& solState = _solState;
    auto startTime = std::chrono::high_resolution_clock::now();

    const Map* map = GetMap();
    int32 mapId = map ? map->GetId() : -1;
    ECLIPSE_LOG_DEBUG("[Eclipse]: Running scripts for state: {}", mapId);

    uint32 count = 0;
    auto& cache = EclipseCache::GetInstance();

    auto executeScripts = [&](const auto& scriptMap) {
        for (const auto& [fileName, script] : scriptMap)
        {
            try
            {
                auto byteCode = cache.GetBytecode(script.filePath);
                if(byteCode.has_value())
                {
                    auto result = solState.load(byteCode->as_string_view());
                    if(result.valid())
                    {
                        result();
                        count++;
                    }
                }
            }
            catch (const sol::error& e)
            {
                ECLIPSE_LOG_ERROR("[Eclipse]: Error executing '{}': {}", script.filePath, e.what());
            }
        }
    };

    executeScripts(EclipseScriptLoader::GetLuaExtensionsMap());
    executeScripts(EclipseScriptLoader::GetLuaScriptsMap());

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    ECLIPSE_LOG_INFO("[Eclipse]: Executed {} Lua scripts in {} µs for map: {}", count, static_cast<uint32>(duration), mapId);
}