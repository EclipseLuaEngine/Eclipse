#include "EclipseScriptLoader.hpp"
#include "EclipseLogger.hpp"
#include "EclipseSolState.hpp"

EclipseSolState::EclipseSolState() : _isInitialized(false)
{
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
bool EclipseSolState::Initialize(int32 mapId)
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
        _solState.add_package_loader(EclipseScriptLoader::GetLuaRequirePreCompiledPath());

        _stateMapId = mapId;
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