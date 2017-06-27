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

bool Logger::Initialize(const std::string& fileName)
{
    try
    {
        _log = spdlog::rotating_logger_mt("file_logger", fileName, 1024 * 1024 * 10, 2);
        _console_log = spdlog::stdout_logger_mt("console_logger");
        _console_log->set_level(spdlog::level::trace);
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


void Logger::SetLogLevel(LOG_LEVEL_ENUM logLevel)
{
    if (_log)
    {
        _log->set_level(static_cast<spdlog::level::level_enum>(logLevel));
    }
}

void Logger::SetConsoleLogLevel(LOG_LEVEL_ENUM logLevel)
{
    if (_console_log)
    {
        _console_log->set_level(static_cast<spdlog::level::level_enum>(logLevel));
    }
}

Logger::pointer Logger::GetLog()
{
    return _log;
}

Logger::pointer Logger::GetConsoleLog()
{
    return _console_log;
}

static const size_t MAX_LOG_LENGTH = 256;
void Logger::LogToConsole(const char* fmt, ...)
{
    std::string strPreMsg;
    va_list args;

    // Declare a moderately sized buffer on the stack that should be
    // large enough to accommodate most log requests.
    int size = MAX_LOG_LENGTH;
    //char stackBuffer[MAX_LOG_LENGTH] = { 0 };
    std::vector<char> dynamicBuffer(MAX_LOG_LENGTH);
    char* str = &dynamicBuffer[0];
    for (;;)
    {
        va_start(args, fmt);
        // Pass one less than size to leave room for NULL terminator
        int needed = vsnprintf(str, size - 1, fmt, args);
        va_end(args);
        // NOTE: Some platforms return -1 when vsnprintf runs out of room, while others return
        // the number of characters actually needed to fill the buffer.
        if (needed >= 0 && needed < size)
        {
            // Successfully wrote buffer. Added a NULL terminator in case it wasn't written.
            str[needed] = '\0';
            break;
        }
        if (needed < 0)
        {
            return;
        }
        size = needed > 0 ? (needed + 1) : (size * 2);
        dynamicBuffer.resize(size);
        str = &dynamicBuffer[0];
    }
    strPreMsg += str;
    strPreMsg += "\n";

#if (TARGET_PLATFORM == PLATFORM_ANDROID)
    __android_log_print(ANDROID_LOG_DEBUG, "FoundationKit", "%s", strPreMsg.c_str());
#elif TARGET_PLATFORM ==  PLATFORM_WINDOWS
    std::wstring wstr = StringUtils::string2UTF8wstring(strPreMsg);
    OutputDebugStringW(wstr.c_str());
    printf("%s", strPreMsg.c_str());
    fflush(stdout);
#else
    // Linux, Mac, iOS, etc
    fprintf(stdout, "%s", strPreMsg.c_str());
    fflush(stdout);
#endif
}

NS_FK_END

