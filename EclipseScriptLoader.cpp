#include "EclipseConfig.hpp"
#include "EclipseIncludes.hpp"
#include "EclipseLogger.hpp"
#include "EclipseScriptLoader.hpp"

std::string EclipseScriptLoader::lua_folderpath;
std::string EclipseScriptLoader::lua_requirepath;
std::string EclipseScriptLoader::lua_requirecpath;
std::string EclipseScriptLoader::lua_precompiledpath;

EclipseScriptLoader::ScriptList EclipseScriptLoader::lua_scripts;
EclipseScriptLoader::ScriptList EclipseScriptLoader::lua_extensions;

/**
 *
 */
bool EclipseScriptLoader::IsValidScriptExtension(std::string& extension)
{
    return extension == ".ext" || extension == ".lua" || extension == ".moon" || extension == ".out" || extension == ".dll" || extension == ".so";
}

/**
 *
 */
void EclipseScriptLoader::ClearLuaScriptPaths()
{
    lua_scripts.clear();
    lua_extensions.clear();

    lua_folderpath = static_cast<std::string>(EclipseConfig::GetInstance().GetScriptPath());
    lua_requirepath = static_cast<std::string>(EclipseConfig::GetInstance().GetRequirePath());
    lua_requirecpath = static_cast<std::string>(EclipseConfig::GetInstance().GetRequireCPath());

#ifndef ECLIPSE_WINDOWS
    if (lua_folderpath[0] == '~')
        if (const char* home = getenv("HOME"))
            lua_folderpath.replace(0, 1, home);
#endif
}

/**
 *
 */
bool EclipseScriptLoader::LoadScriptPaths()
{
    try
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        ClearLuaScriptPaths();

        GetScripts(lua_folderpath);

        if(!lua_requirepath.empty())
            lua_requirepath.erase(lua_requirepath.end() - 1);

        if(!lua_requirecpath.empty())
            lua_requirecpath.erase(lua_requirecpath.end() - 1);

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

        uint32 totalScripts = lua_scripts.size() + lua_extensions.size();
        ECLIPSE_LOG_INFO("[Eclipse]: Loaded {} scripts in {} µs", totalScripts, static_cast<uint32>(duration));
        return true;
    }
    catch (const std::exception& e)
    {
        ECLIPSE_LOG_ERROR("[Eclipse]: Exception loading directory '{}': {}", lua_folderpath, e.what());
        return false;
    }
}

/**
 *
 */
void EclipseScriptLoader::AddScriptPath(std::string filename, const std::string& fullpath)
{
    ECLIPSE_LOG_DEBUG("[Eclipse]: Trying to add {}, {}", filename, fullpath);

    try
    {
        // split file name
        std::size_t extDot = filename.find_last_of('.');
        if (extDot == std::string::npos)
            return;

        std::string ext = filename.substr(extDot);
        filename = filename.substr(0, extDot);

        if(!IsValidScriptExtension(ext))
            return;

        bool extension = ext == ".ext";

        LuaScript script;
        script.fileext = ext;
        script.filename = filename;
        script.filepath = fullpath;
        script.modulepath = fullpath.substr(0, fullpath.length() - filename.length() - ext.length());

        if (extension)
            lua_extensions.push_back(script);
        else
            lua_scripts.push_back(script);

        ECLIPSE_LOG_DEBUG("[Eclipse]: AddScriptPath add path `{}`", fullpath);
    }
    catch(const std::exception& e)
    {
        ECLIPSE_LOG_ERROR("[Eclipse]: Exception when adding script `{}`: {}", fullpath, e.what());
    }
}

/**
 *
 */
void EclipseScriptLoader::GetScripts(const std::string& path)
{
    ECLIPSE_LOG_DEBUG("[Eclipse]: GetScripts from path `{}`", path);

    try
    {
        boost::filesystem::path someDir(path);
        boost::filesystem::directory_iterator end_iter;

        if (boost::filesystem::exists(someDir) && boost::filesystem::is_directory(someDir))
        {
            lua_requirepath +=
                path + "/?.lua;" +
                path + "/?.moon;" +
                path + "/?.ext;";

            lua_requirecpath +=
                path + "/?.dll;" +
                path + "/?.so;";

            lua_precompiledpath +=
                path + "/?.out;";

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
                    GetScripts(fullpath);
                    continue;
                }

                if (boost::filesystem::is_regular_file(dir_iter->status()))
                {
                    // was file, try add
                    std::string filename = dir_iter->path().filename().generic_string();
                    ECLIPSE_LOG_DEBUG("[Eclipse]: Script {} path {}", filename, fullpath);
                    AddScriptPath(filename, fullpath);
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        ECLIPSE_LOG_ERROR("[Eclipse]: Exception when getting scripts '{}': {}", path, e.what());
    }
}