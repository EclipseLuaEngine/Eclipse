#ifndef ECLIPSE_LUA_COMPILER_HPP
#define ECLIPSE_LUA_COMPILER_HPP

#include "EclipseIncludes.hpp"

class EclipseCompiler
{
    public:
        EclipseCompiler() = default;
        ~EclipseCompiler() = default;

        static std::optional<sol::bytecode> CompileLuaToByteCode(sol::state& compilerState, const std::string& filePath);
        static sol::bytecode CompileMoonToByteCode(const std::string& filePath);
};

#endif // ECLIPSE_LUA_COMPILER_HPP