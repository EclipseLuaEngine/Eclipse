#ifndef ECLIPSE_LOGGER_HPP
#define ECLIPSE_LOGGER_HPP

#include "Log.h"

class EclipseLogger
{
    public:
        static EclipseLogger& GetInstance();

    private:
        EclipseLogger() = default;
        ~EclipseLogger() = default;
        EclipseLogger(const EclipseLogger&) = delete;
        EclipseLogger& operator=(const EclipseLogger&) = delete;
};

#define ECLIPSE_LOG_INFO(...)     LOG_INFO("server.eclipse", __VA_ARGS__);
#define ECLIPSE_LOG_WARN(...)     LOG_WARN("server.eclipse", __VA_ARGS__);
#define ECLIPSE_LOG_ERROR(...)    LOG_ERROR("server.eclipse", __VA_ARGS__);
#define ECLIPSE_LOG_DEBUG(...)    LOG_DEBUG("server.eclipse", __VA_ARGS__);
#define ECLIPSE_LOG_TRACE(...)    LOG_TRACE("server.eclipse", __VA_ARGS__);


#endif // ECLIPSE_LOGGER_HPP

