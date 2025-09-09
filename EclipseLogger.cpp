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