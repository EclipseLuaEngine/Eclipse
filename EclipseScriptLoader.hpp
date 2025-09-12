#ifndef ECLIPSE_SCRIPT_LOADER_HPP
#define ECLIPSE_SCRIPT_LOADER_HPP

#include "EclipseIncludes.hpp"
#include <boost/filesystem.hpp>

struct LuaScript
{
    std::string fileExt;
    std::string fileName;
    std::string filePath;

    LuaScript() = default;
    LuaScript(std::string ext, std::string name, std::string path)
        : fileExt(std::move(ext)), fileName(std::move(name)), filePath(std::move(path)) {}
};

class EclipseScriptLoader
{
    public:
        typedef std::unordered_map<std::string, LuaScript> ScriptMap;

        static bool IsValidScriptExtension(std::string& extension);

        static bool LoadScriptPaths();
        static void ProcessScript(sol::state& tempstate, const std::string& filename, const std::string& fullpath);
        static void GetScripts(sol::state& tempState, const std::string& path);

        static bool CompileScript(sol::state& tempState, LuaScript& script);

        static const std::string& GetLuaRequirePath()       { return lua_requirepath; }
        static const std::string& GetLuaRequireCPath()      { return lua_requirecpath; }
        static const std::string& GetLuaFolderPath()        { return lua_folderpath; }
        static const ScriptMap& GetLuaExtensionsMap()       { return lua_extensionsMap; }
        static const ScriptMap& GetLuaScriptsMap()          { return lua_scriptsMap; }

        static void ClearLuaScriptPaths();

    private:
        EclipseScriptLoader() = delete;
        ~EclipseScriptLoader() = default;
        EclipseScriptLoader(const EclipseScriptLoader&) = delete;
        EclipseScriptLoader& operator=(const EclipseScriptLoader&) = delete;

        static std::string lua_folderpath;
        static std::string lua_requirepath;
        static std::string lua_requirecpath;

        static ScriptMap lua_extensionsMap;
        static ScriptMap lua_scriptsMap;
};

#endif // ECLIPSE_SCRIPT_LOADER_HPP