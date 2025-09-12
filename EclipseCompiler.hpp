/*
 * Copyright (C) 2010 - 2024 Eluna Lua Engine <https://elunaluaengine.github.io/>
 * This program is free software licensed under GPL version 3
 * Please see the included LICENSE.md for more information
 */

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