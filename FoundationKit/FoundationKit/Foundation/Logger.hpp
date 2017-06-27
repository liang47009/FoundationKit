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
    bool Initialize(const std::string& fileName);
    void SetLogLevel(LOG_LEVEL_ENUM logLevel);
    void SetConsoleLogLevel(LOG_LEVEL_ENUM logLevel);
    pointer GetLog();
    pointer GetConsoleLog();
    static void LogToConsole(const char* fmt, ...);
private:
    pointer _log;
    pointer _console_log;
};

template<typename... Args>
static inline void LOG_TRACE(const char* fmt, const Args&... args)
{
    Logger::GetInstance()->GetLog()->trace(fmt, args...);
}

template<typename... Args>
static inline void LOG_INFO(const char* fmt, const Args&... args)
{
    Logger::GetInstance()->GetLog()->info(fmt, args...);
}

template<typename... Args>
static inline void LOG_WARN(const char* fmt, const Args&... args)
{
    Logger::GetInstance()->GetLog()->warn(fmt, args...);
}

template<typename... Args>
static inline void LOG_ERROR(const char* fmt, const Args&... args)
{
    Logger::GetInstance()->GetLog()->error(fmt, args...);
}

template<typename... Args>
static inline void LOG_CRITICAL(const char* fmt, const Args&... args)
{
    Logger::GetInstance()->GetLog()->critical(fmt, args...);
}

template<typename... Args>
static inline void LOG_DEBUG(const char* fmt, const Args&... args)
{
    Logger::GetInstance()->GetConsoleLog()->debug(fmt, args...);
}

#define LOG_CONSOLE(fmt, ...) FoundationKit::Logger::LogToConsole(fmt, ##__VA_ARGS__)

NS_FK_END

#endif // FOUNDATIONKIT_LOGGER_HPP