/****************************************************************************
  Copyright (c) 2015 libo All rights reserved.

  losemymind.libo@gmail.com

****************************************************************************/
#include <vector>
#include <stdarg.h>
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/Logger.hpp"
#if (TARGET_PLATFORM == PLATFORM_WINDOWS)
#include <Winsock2.h>
#endif

#if (TARGET_PLATFORM == PLATFORM_ANDROID)
#include <android/log.h>
#endif


NS_FK_BEGIN

Logger::~Logger()
{
    _log->flush();
    _console_log->flush();
    //spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l)
    //{
    //    l->info("End of Logger.");
    //});
    //spdlog::drop_all();
}


Logger::Logger()
{

}

bool Logger::init(const std::string& fileName)
{
    try
    {
        _log = spdlog::rotating_logger_mt("file_logger", fileName, 1024 * 1024 * 10, 2);
        _console_log = spdlog::stdout_logger_mt("console_logger");
    }
    catch (std::exception&)
    {
        return false;
    }
    catch (...)
    {
        return false;
    }

    return true;
}


Logger::pointer Logger::get_log()
{
    return _log;
}

Logger::pointer Logger::get_console_log()
{
    return _console_log;
}

NS_FK_END

