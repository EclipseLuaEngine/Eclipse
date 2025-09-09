#ifndef ECLIPSE_SCRIPT_LOADER_HPP
#define ECLIPSE_SCRIPT_LOADER_HPP

#include <boost/filesystem.hpp>

struct LuaScript
{
    std::string fileext;
    std::string filename;
    std::string filepath;
    std::string modulepath;
    LuaScript() {}
};

class EclipseScriptLoader
{
    public:
        typedef std::list<LuaScript> ScriptList;

        static bool IsValidScriptExtension(std::string& extension);

        static bool LoadScriptPaths();
        static void AddScriptPath(std::string filename, const std::string& fullpath);
        static void GetScripts(const std::string& path);

        static std::string GetLuaRequirePath()              { return lua_requirepath; }
        static std::string GetLuaRequireCPath()             { return lua_requirecpath; }
        static std::string GetLuaFolderPath()               { return lua_folderpath; }
        static std::string GetLuaRequirePreCompiledPath()   { return lua_precompiledpath; }
        static ScriptList GetLuaScripts()                   { return lua_scripts; }
        static ScriptList GetLuaExtensions()                { return lua_extensions; }

        static void ClearLuaScriptPaths();

    private:
        EclipseScriptLoader() = delete;
        ~EclipseScriptLoader() = default;
        EclipseScriptLoader(const EclipseScriptLoader&) = delete;
        EclipseScriptLoader& operator=(const EclipseScriptLoader&) = delete;

        static std::string lua_folderpath;
        static std::string lua_requirepath;
        static std::string lua_requirecpath;
        static std::string lua_precompiledpath;

        static ScriptList lua_scripts;
        static ScriptList lua_extensions;
};

#endif // ECLIPSE_SCRIPT_LOADER_HPP