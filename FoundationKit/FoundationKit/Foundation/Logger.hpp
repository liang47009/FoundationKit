/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_LOGGER_HPP
#define FOUNDATIONKIT_LOGGER_HPP

#pragma once
#include "FoundationKit/forward.hpp"
#include "FoundationKit/Foundation/Singleton.hpp"
#include "FoundationKit/Platform/Platform.hpp"
#include "spdlog/spdlog.h"
#if (TARGET_PLATFORM == PLATFORM_ANDROID)
#include <android/log.h>
#elif (TARGET_PLATFORM == PLATFORM_WINDOWS)
#include <Windows.h>
#endif


NS_FK_BEGIN

typedef enum
{
    trace = 0,
    debug = 1,
    info = 2,
    warn = 3,
    err = 4,
    critical = 5,
    off = 6
} LOG_LEVEL_ENUM;

class Logger : public Singleton<Logger>
{
    friend class Singleton < Logger >;
    Logger();
public:
    using pointer = std::shared_ptr<spdlog::logger>;
public:
    virtual~Logger();
    bool init(const std::string& fileName);
    void setLogLevel(LOG_LEVEL_ENUM logLevel);
    void setConsoleLogLevel(LOG_LEVEL_ENUM logLevel);
    pointer getLog();
    pointer getConsoleLog();
private:
    pointer _log;
    pointer _console_log;
};

template<typename... Args>
static inline void LOG_TRACE(const char* fmt, const Args&... args)
{
    Logger::getInstance()->getLog()->trace(fmt, args...);
}

template<typename... Args>
static inline void LOG_INFO(const char* fmt, const Args&... args)
{
    Logger::getInstance()->getLog()->info(fmt, args...);
}

template<typename... Args>
static inline void LOG_WARN(const char* fmt, const Args&... args)
{
    Logger::getInstance()->getLog()->warn(fmt, args...);
}

template<typename... Args>
static inline void LOG_ERROR(const char* fmt, const Args&... args)
{
    Logger::getInstance()->getLog()->error(fmt, args...);
}

template<typename... Args>
static inline void LOG_CRITICAL(const char* fmt, const Args&... args)
{
    Logger::getInstance()->getLog()->critical(fmt, args...);
}

template<typename... Args>
static inline void LOG_DEBUG(const char* fmt, const Args&... args)
{
    Logger::getInstance()->getConsoleLog()->debug(fmt, args...);
}

NS_FK_END

#endif // FOUNDATIONKIT_LOGGER_HPP