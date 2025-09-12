/*
 * Copyright (C) 2010 - 2024 Eluna Lua Engine <https://elunaluaengine.github.io/>
 * This program is free software licensed under GPL version 3
 * Please see the included LICENSE.md for more information
 */

#ifndef ECLIPSE_LUA_CACHE_HPP
#define ECLIPSE_LUA_CACHE_HPP

#include "EclipseIncludes.hpp"

struct CacheEntry
{
    sol::bytecode bytecode;
    std::time_t last_modified;

    CacheEntry() : last_modified(0) {}
    CacheEntry(const sol::bytecode& code, std::time_t modTime)
        : bytecode(code), last_modified(modTime) {}
};

enum EclipseScriptCacheState
{
    SCRIPT_CACHE_NONE = 0,
    SCRIPT_CACHE_REINIT = 1,
    SCRIPT_CACHE_LOADING = 2,
    SCRIPT_CACHE_READY = 3
};

class EclipseCache
{
    public:
        static EclipseCache& GetInstance();

        std::optional<sol::bytecode> GetBytecode(const std::string& filePath);
        void StoreByteCode(const std::string& filePath, sol::bytecode bytecode);

        CacheEntry& GetCacheEntry(const std::string& filePath) { return _cache[filePath]; }

        std::time_t GetFileWriteTime(const std::string& filePath);
        std::time_t GetCacheWriteTime(const std::string& filePath);
        bool IsScriptModified(const std::string& filePath);

        bool IsInCache(const std::string& filePath) {
            auto it = _cache.find(filePath);
            return it != _cache.end() && !it->second.bytecode.as_string_view().empty();
        }

        void InvalidateScript(const std::string& filePath);
        void InvalidateAllScripts();

        uint8 GetCacheState() { return _cacheState; }
        void SetCacheState(uint8 cacheState) { _cacheState = cacheState; }

    private:
        EclipseCache() = default;
        ~EclipseCache() = default;
        EclipseCache(const EclipseCache&) = delete;
        EclipseCache& operator=(const EclipseCache&) = delete;

    private:
        std::unordered_map<std::string, CacheEntry> _cache;
        std::atomic<uint8> _cacheState;
};

#endif //ECLIPSE_LUA_CACHE_HPP