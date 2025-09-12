/*
 * Copyright (C) 2010 - 2024 Eluna Lua Engine <https://elunaluaengine.github.io/>
 * This program is free software licensed under GPL version 3
 * Please see the included LICENSE.md for more information
 */

#ifndef ECLIPSE_CONFIG_HPP
#define ECLIPSE_CONFIG_HPP

#include "ConfigValueCache.h"

enum class EclipseConfigValues : uint32
{
    // Boolean
    ENABLED = 0,
    AUTORELOAD_ENABLED,
    BYTECODE_CACHE_ENABLED,

    // String
    SCRIPT_PATH,
    REQUIRE_PATH,
    REQUIRE_CPATH,

    // Number
    AUTORELOAD_INTERVAL,

    CONFIG_VALUE_COUNT
};

class EclipseConfig final : public ConfigValueCache<EclipseConfigValues>
{
    public:
        static EclipseConfig& GetInstance();

        void Initialize(bool reload = false);

        bool IsEclipseEnabled() const { return GetConfigValue<bool>(EclipseConfigValues::ENABLED); }
        bool IsAutoReloadEnabled() const { return GetConfigValue<bool>(EclipseConfigValues::AUTORELOAD_ENABLED); }
        bool IsByteCodeCacheEnabled() const { return GetConfigValue<bool>(EclipseConfigValues::BYTECODE_CACHE_ENABLED); }

        std::string_view GetScriptPath() const { return GetConfigValue(EclipseConfigValues::SCRIPT_PATH); }
        std::string_view GetRequirePath() const { return GetConfigValue(EclipseConfigValues::REQUIRE_PATH); }
        std::string_view GetRequireCPath() const { return GetConfigValue(EclipseConfigValues::REQUIRE_CPATH); }

        uint32 GetAutoReloadInterval() const { return GetConfigValue<uint32>(EclipseConfigValues::AUTORELOAD_INTERVAL); }

    protected:
        void BuildConfigCache() override;

    private:
        EclipseConfig();
        ~EclipseConfig() = default;
        EclipseConfig(const EclipseConfig&) = delete;
        EclipseConfig& operator=(const EclipseConfig&) = delete;
};


#endif // ECLIPSE_CONFIG_HPP
