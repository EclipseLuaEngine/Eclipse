#include "EclipseIncludes.hpp"
#include "EclipseConfig.hpp"

EclipseConfig& EclipseConfig::GetInstance()
{
    static EclipseConfig instance;
    return instance;
}

EclipseConfig::EclipseConfig() : ConfigValueCache<EclipseConfigValues>(EclipseConfigValues::CONFIG_VALUE_COUNT)
{
}

/**
 *
 */
void EclipseConfig::Initialize(bool reload)
{
    ConfigValueCache<EclipseConfigValues>::Initialize(reload);
}

/**
 *
 */
void EclipseConfig::BuildConfigCache()
{
    SetConfigValue<bool>(EclipseConfigValues::ENABLED,                    "Eclipse.Enabled",            "false");
    SetConfigValue<bool>(EclipseConfigValues::AUTORELOAD_ENABLED,         "Eclipse.AutoReload",         "false");
    SetConfigValue<bool>(EclipseConfigValues::BYTECODE_CACHE_ENABLED,     "Eclipse.BytecodeCache",      "false");

    SetConfigValue<std::string>(EclipseConfigValues::SCRIPT_PATH,         "Eclipse.ScriptPath",         "lua_scripts");
    SetConfigValue<std::string>(EclipseConfigValues::REQUIRE_PATH,        "Eclipse.RequirePaths",       "");
    SetConfigValue<std::string>(EclipseConfigValues::REQUIRE_CPATH,       "Eclipse.RequireCPaths",      "");

    SetConfigValue<uint32>(EclipseConfigValues::AUTORELOAD_INTERVAL,      "Eclipse.AutoReloadInterval", 1);
}