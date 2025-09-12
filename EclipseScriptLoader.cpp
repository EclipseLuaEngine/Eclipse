/*
 * Copyright (C) 2010 - 2024 Eluna Lua Engine <https://elunaluaengine.github.io/>
 * This program is free software licensed under GPL version 3
 * Please see the included LICENSE.md for more information
 */

#include "EclipseCompiler.hpp"
#include "EclipseCache.hpp"
#include "EclipseConfig.hpp"
#include "EclipseLogger.hpp"
#include "EclipseScriptLoader.hpp"

std::string EclipseScriptLoader::lua_folderpath;
std::string EclipseScriptLoader::lua_requirepath;
std::string EclipseScriptLoader::lua_requirecpath;

EclipseScriptLoader::ScriptMap EclipseScriptLoader::lua_extensionsMap;
EclipseScriptLoader::ScriptMap EclipseScriptLoader::lua_scriptsMap;

/**
 *
 */
bool EclipseScriptLoader::IsValidScriptExtension(std::string& extension)
{
    static const std::unordered_set<std::string> validExtensions{
        ".ext", ".lua", ".moon", ".out", ".dll", ".so"
    };
    return validExtensions.count(extension) > 0;
}


/**
 *
 */
void EclipseScriptLoader::ClearLuaScriptPaths()
{
    lua_extensionsMap.clear();
    lua_scriptsMap.clear();

    const auto& config = EclipseConfig::GetInstance();
    lua_folderpath = config.GetScriptPath();
    lua_requirepath = config.GetRequirePath();
    lua_requirecpath = config.GetRequireCPath();

    lua_requirepath.reserve(1024);
    lua_requirecpath.reserve(512);

#ifndef ECLIPSE_WINDOWS
    if (!lua_folderpath.empty() && lua_folderpath[0] == '~')
        if (const char* home = getenv("HOME"))
            lua_folderpath.replace(0, 1, home);
#endif
}

/**
 *
 */
bool EclipseScriptLoader::LoadScriptPaths()
{
    EclipseCache& eclipseCache = EclipseCache::GetInstance();

    if(eclipseCache.GetCacheState() != SCRIPT_CACHE_REINIT && eclipseCache.GetCacheState() != SCRIPT_CACHE_NONE)
        return false;

    eclipseCache.SetCacheState(SCRIPT_CACHE_LOADING);

    auto startTime = std::chrono::high_resolution_clock::now();

    ClearLuaScriptPaths();

    sol::state tempState = sol::state();
    tempState.open_libraries(
        sol::lib::base,
        sol::lib::package
    );

    GetScripts(tempState, lua_folderpath);

    if(!lua_requirepath.empty())
        lua_requirepath.erase(lua_requirepath.end() - 1);

    if(!lua_requirecpath.empty())
        lua_requirecpath.erase(lua_requirecpath.end() - 1);

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

    ECLIPSE_LOG_INFO("[Eclipse]: Loaded {} scripts in {} µs", lua_scriptsMap.size(), static_cast<uint32>(duration));

    eclipseCache.SetCacheState(SCRIPT_CACHE_READY);
    return true;
}

/**
 *
 */
bool EclipseScriptLoader::CompileScript(sol::state& tempstate, LuaScript& script)
{
    if(script.fileExt == ".lua" || script.fileExt == ".ext")
    {
        auto& cache = EclipseCache::GetInstance();
        std::optional<sol::bytecode> bytecode;

        if (cache.IsScriptModified(script.filePath))
        {
            bytecode = EclipseCompiler::CompileLuaToByteCode(tempstate, script.filePath);
            if (bytecode.has_value())
                cache.StoreByteCode(script.filePath, bytecode.value());
            else
                return false;
        }
    }
    return true;
}

/**
 *
 */
void EclipseScriptLoader::ProcessScript(sol::state& tempstate, const std::string& filename, const std::string& fullpath)
{
    ECLIPSE_LOG_DEBUG("[Eclipse]: Processing script: {}", fullpath);

    try
    {
        std::size_t extDot = filename.find_last_of('.');
        if (extDot == std::string::npos)
            return;

        std::string ext = filename.substr(extDot);
        if(!IsValidScriptExtension(ext))
            return;

        std::string scriptName = filename.substr(0, extDot);
        LuaScript script(std::move(ext), std::move(scriptName), fullpath);

        if(!CompileScript(tempstate, script))
            return;

        bool isExtension = script.fileExt == ".ext";
        if (isExtension)
            lua_extensionsMap[script.fileName] = std::move(script);
        else
            lua_scriptsMap[script.fileName] = std::move(script);
        
        ECLIPSE_LOG_DEBUG("[Eclipse]: Added script: {} (extension: {})", fullpath, isExtension);
    }
    catch(const std::exception& e)
    {
        ECLIPSE_LOG_ERROR("[Eclipse]: Failed to process script '{}': {}", fullpath, e.what());
    }
}

/**
 *
 */
void EclipseScriptLoader::GetScripts(sol::state& tempState, const std::string& path)
{
    ECLIPSE_LOG_DEBUG("[Eclipse]: GetScripts from path `{}`", path);

    try
    {
        boost::filesystem::path someDir(path);
        boost::filesystem::directory_iterator end_iter;

        if (boost::filesystem::exists(someDir) && boost::filesystem::is_directory(someDir))
        {
            lua_requirepath += path + "/?.lua;" + path + "/?.moon;" + path + "/?.ext;" + path + "/?.out;";
            lua_requirecpath += path + "/?.dll;" + path + "/?.so;";

            for (boost::filesystem::directory_iterator dir_iter(someDir); dir_iter != end_iter; ++dir_iter)
            {
                std::string fullpath = dir_iter->path().generic_string();

                // Check if file is hidden
#ifdef ECLIPSE_WINDOWS
                DWORD dwAttrib = GetFileAttributes(fullpath.c_str());
                if (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_HIDDEN))
                    continue;
#else
                std::string name = dir_iter->path().filename().generic_string().c_str();
                if (name[0] == '.')
                    continue;
#endif

                // load subfolder
                if (boost::filesystem::is_directory(dir_iter->status()))
                {
                    GetScripts(tempState, fullpath);
                    continue;
                }

                if (boost::filesystem::is_regular_file(dir_iter->status()))
                {
                    std::string fileName = dir_iter->path().filename().generic_string();
                    ProcessScript(tempState, fileName, fullpath);
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        ECLIPSE_LOG_ERROR("[Eclipse]: Exception when getting scripts '{}': {}", path, e.what());
    }
}