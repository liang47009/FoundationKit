/****************************************************************************
Copyright (c) 2013-2014 libo.

losemymind.libo@gmail.com

****************************************************************************/
#include <vector>
#include <stdarg.h>
#include "Logger.h"
#include "FoundationKit/Foundation/StringUtils.h"

#if (FK_TARGET_PLATFORM == FK_PLATFORM_WIN32)
#include <Winsock2.h>
#endif

#if (FK_TARGET_PLATFORM == FK_PLATFORM_ANDROID)
#include <android/log.h>
#endif


NS_FK_BEGIN

static const size_t MAX_LOG_LENGTH = 1024;
static const char*  LevelMsg[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};


Logger::State::State():enabled(true){}

void Logger::log( Level level, const char* message, ... )
{
    Logger::State& state = _states[level];
    if (!state.enabled)
        return;
    std::string strPreMsg = LevelMsg[level];
    strPreMsg += ":";
    va_list args;
    va_start(args, message);
    // Declare a moderately sized buffer on the stack that should be
    // large enough to accommodate most log requests.
    int size = MAX_LOG_LENGTH;
    //char stackBuffer[MAX_LOG_LENGTH] = { 0 };
    std::vector<char> dynamicBuffer(MAX_LOG_LENGTH);
    char* str = &dynamicBuffer[0];
    for (;;)
    {
        // Pass one less than size to leave room for NULL terminator
        int needed = vsnprintf(str, size - 1, message, args);

        // NOTE: Some platforms return -1 when vsnprintf runs out of room, while others return
        // the number of characters actually needed to fill the buffer.
        if (needed >= 0 && needed < size)
        {
            // Successfully wrote buffer. Added a NULL terminator in case it wasn't written.
            str[needed] = '\0';
            break;
        }
        size = needed > 0 ? (needed + 1) : (size * 2);
        dynamicBuffer.resize(size);
        str = &dynamicBuffer[0];
    }
    strPreMsg += str;
    strPreMsg += "\n";

#if (FK_TARGET_PLATFORM == FK_PLATFORM_ANDROID)
    __android_log_print(ANDROID_LOG_DEBUG, "FoundationKit", "%s", strPreMsg.c_str());
#elif FK_TARGET_PLATFORM ==  FK_PLATFORM_WIN32 || FK_TARGET_PLATFORM == FK_PLATFORM_WINRT || FK_TARGET_PLATFORM == FK_PLATFORM_WP8
    std::wstring wstr = StringUtils::string2UTF8wstring(strPreMsg.c_str());
    OutputDebugStringW(wstr.c_str());
    printf("%s", strPreMsg.c_str());
    fflush(stdout);
#else
    // Linux, Mac, iOS, etc
    fprintf(stdout, "%s", strPreMsg.c_str());
    fflush(stdout);
#endif
    va_end(args);
}

bool Logger::isEnabled( Level level )
{
    return _states[level].enabled;
}

void Logger::setEnabled( Level level, bool enabled )
{
    _states[level].enabled = enabled;
}


NS_FK_END

extern void _log_(const char* message, ...)
{
    std::string strPreMsg;
    va_list args;
    va_start(args, message);
    // Declare a moderately sized buffer on the stack that should be
    // large enough to accommodate most log requests.
    int size = NS_NAME::MAX_LOG_LENGTH;
    //char stackBuffer[MAX_LOG_LENGTH] = { 0 };
    std::vector<char> dynamicBuffer(NS_NAME::MAX_LOG_LENGTH);
    char* str = &dynamicBuffer[0];
    for (;;)
    {
        // Pass one less than size to leave room for NULL terminator
        int needed = vsnprintf(str, size - 1, message, args);

        // NOTE: Some platforms return -1 when vsnprintf runs out of room, while others return
        // the number of characters actually needed to fill the buffer.
        if (needed >= 0 && needed < size)
        {
            // Successfully wrote buffer. Added a NULL terminator in case it wasn't written.
            str[needed] = '\0';
            break;
        }
        size = needed > 0 ? (needed + 1) : (size * 2);
        dynamicBuffer.resize(size);
        str = &dynamicBuffer[0];
    }
    strPreMsg += str;
    strPreMsg += "\n";

#if (FK_TARGET_PLATFORM == FK_PLATFORM_ANDROID)
    __android_log_print(ANDROID_LOG_DEBUG, "FoundationKit", "%s", strPreMsg.c_str());
#elif FK_TARGET_PLATFORM ==  FK_PLATFORM_WIN32 || FK_TARGET_PLATFORM == FK_PLATFORM_WINRT || FK_TARGET_PLATFORM == FK_PLATFORM_WP8
    std::wstring wstr = NS_NAME::StringUtils::string2UTF8wstring(strPreMsg.c_str());
    OutputDebugStringW(wstr.c_str());
    printf("%s", strPreMsg.c_str());
    fflush(stdout);
#else
    // Linux, Mac, iOS, etc
    fprintf(stdout, "%s", strPreMsg.c_str());
    fflush(stdout);
#endif
    va_end(args);
}
