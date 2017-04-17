/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_LOGGER_HPP
#define FOUNDATIONKIT_LOGGER_HPP

#pragma once
#include "FoundationKit/forward.hpp"
#include "FoundationKit/Foundation/Singleton.hpp"
#include "spdlog/spdlog.h"
#if (TARGET_PLATFORM == PLATFORM_ANDROID)
#include <android/log.h>
#elif (TARGET_PLATFORM == PLATFORM_WINDOWS)
#include <Windows.h>
#endif


NS_FK_BEGIN

class Logger : public Singleton<Logger>
{
    friend class Singleton < Logger >;
    Logger();
public:
    using pointer = std::shared_ptr<spdlog::logger>;
public:
    virtual~Logger();
    bool init(const std::string& fileName);
    pointer get_log();
    pointer get_console_log();
private:
    pointer _log;
    pointer _console_log;
};

template<typename... Args>
static inline void LOG_TRACE(const char* fmt, const Args&... args)
{
    Logger::getInstance()->get_log()->trace(fmt, args...);
}

template<typename... Args>
static inline void LOG_INFO(const char* fmt, const Args&... args)
{
    Logger::getInstance()->get_log()->info(fmt, args...);
}

template<typename... Args>
static inline void LOG_WARN(const char* fmt, const Args&... args)
{
    Logger::getInstance()->get_log()->warn(fmt, args...);
}

template<typename... Args>
static inline void LOG_ERROR(const char* fmt, const Args&... args)
{
    Logger::getInstance()->get_log()->error(fmt, args...);
}

template<typename... Args>
static inline void LOG_CRITICAL(const char* fmt, const Args&... args)
{
    Logger::getInstance()->get_log()->critical(fmt, args...);
}

template<typename... Args>
static inline void LOG_DEBUG(const char* fmt, const Args&... args)
{
    Logger::getInstance()->get_log()->set_level(spdlog::level::debug);
    Logger::getInstance()->get_log()->debug(fmt, args...);
    Logger::getInstance()->get_console_log()->set_level(spdlog::level::debug);
    Logger::getInstance()->get_console_log()->debug(fmt, args...);
}

NS_FK_END

#endif // FOUNDATIONKIT_LOGGER_HPP