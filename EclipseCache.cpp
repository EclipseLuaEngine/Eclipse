#include "EclipseScriptLoader.hpp"
#include "EclipseLogger.hpp"
#include "EclipseCache.hpp"

EclipseCache& EclipseCache::GetInstance()
{
    static EclipseCache instance;
    return instance;
}

std::time_t EclipseCache::GetFileWriteTime(const std::string& filePath)
{
    struct stat fileInfo;
    if (stat(filePath.c_str(), &fileInfo) == 0)
        return fileInfo.st_mtime;

    return 0;
}

std::time_t EclipseCache::GetCacheWriteTime(const std::string& filePath)
{
    auto it = _cache.find(filePath);
    if (it != _cache.end())
        return it->second.last_modified;

    return 0;
}

bool EclipseCache::IsScriptModified(const std::string& filePath)
{
    std::time_t cacheTime = GetCacheWriteTime(filePath);
    std::time_t fileTime = GetFileWriteTime(filePath);

    return fileTime > cacheTime;
}

std::optional<sol::bytecode> EclipseCache::GetBytecode(const std::string& filePath)
{
    auto it = _cache.find(filePath);
    if(it == _cache.end())
        return std::nullopt;

    CacheEntry cacheEntry = it->second;

    if (IsScriptModified(filePath))
    {
        InvalidateScript(filePath);
        return std::nullopt;
    }

    return cacheEntry.bytecode;
}

void EclipseCache::InvalidateScript(const std::string& filePath)
{
    _cache.erase(filePath);
    ECLIPSE_LOG_INFO("[Eclipse]: Invalidated cache for script: {}", filePath);
}

void EclipseCache::StoreByteCode(const std::string& filePath, sol::bytecode bytecode)
{
    std::time_t modTime = GetFileWriteTime(filePath);
    _cache[filePath] = CacheEntry(std::move(bytecode), modTime);
}

void EclipseCache::InvalidateAllScripts()
{
    _cache.clear();
}