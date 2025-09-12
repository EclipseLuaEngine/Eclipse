/*
 * Copyright (C) 2010 - 2024 Eluna Lua Engine <https://elunaluaengine.github.io/>
 * This program is free software licensed under GPL version 3
 * Please see the included LICENSE.md for more information
 */

#include "EclipseIncludes.hpp"
#include "EclipseLogger.hpp"

EclipseLogger& EclipseLogger::GetInstance()
{
    static EclipseLogger instance;
    return instance;
}

/**
 *
 */