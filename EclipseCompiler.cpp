/*
 * Copyright (C) 2010 - 2024 Eluna Lua Engine <https://elunaluaengine.github.io/>
 * This program is free software licensed under GPL version 3
 * Please see the included LICENSE.md for more information
 */

#include "EclipseCompiler.hpp"
#include "EclipseLogger.hpp"

std::optional<sol::bytecode> EclipseCompiler::CompileLuaToByteCode(sol::state& compilerState, const std::string& filePath)
{
    try
    {
        auto startTime = std::chrono::high_resolution_clock::now();

        sol::load_result loaded_script = compilerState.load_file(filePath);

        if (!loaded_script.valid())
        {
            sol::error err = loaded_script;
            ECLIPSE_LOG_ERROR("[Eclipse]: Error loading script `{}`: {}", filePath, err.what());
            return std::nullopt;
        }

        sol::protected_function target = loaded_script.get<sol::protected_function>();
        sol::bytecode bytecode = target.dump();

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        ECLIPSE_LOG_DEBUG("[Eclipse]: Successfully compiled `{}` to bytecode in {} µs", filePath, static_cast<uint32>(duration));

        return bytecode;
    }
    catch (const sol::error& e)
    {
        ECLIPSE_LOG_ERROR("[Eclipse]: Sol2 error compiling `{}`: {}", filePath, e.what());
    }
    catch (const std::exception& e)
    {
        ECLIPSE_LOG_ERROR("[Eclipse]: Exception compiling `{}`: {}", filePath, e.what());
    }
    catch (...)
    {
        ECLIPSE_LOG_ERROR("[Eclipse]: Unknown error compiling `{}`", filePath);
    }
    return std::nullopt;
}