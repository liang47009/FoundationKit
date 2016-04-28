#include "GenericPlatformMacros.h"
#include "FoundationKit/Foundation/StringUtils.h"
#include <cstdarg>
#include <cstdio>
#if (TARGET_PLATFORM == PLATFORM_ANDROID)
#include <android/log.h>
#elif (TARGET_PLATFORM == PLATFORM_WIN32)
#include <Windows.h>
#endif
USING_NS_FK;
void __log__(const char* file, int line, const char* message, ...)
{
    static const size_t MAX_LOG_LENGTH = 1024;
    // Declare a moderately sized buffer on the stack that should be
    // large enough to accommodate most log requests.
    int size = MAX_LOG_LENGTH;
    //char stackBuffer[MAX_LOG_LENGTH] = { 0 };
    std::vector<char> dynamicBuffer(MAX_LOG_LENGTH);
    char* str = &dynamicBuffer[0];
    std::string logMessage;
    logMessage += file ? file : "";
    logMessage += ":";
    logMessage += std::to_string(line);
    va_list args;
    for (;;)
    {
        va_start(args, message);
        // Pass one less than size to leave room for NULL terminator
        int needed = vsnprintf(str, size - 1, message, args);
        va_end(args);
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
    logMessage += str;

#if (TARGET_PLATFORM == PLATFORM_ANDROID)
    __android_log_print(ANDROID_LOG_INFO, "LoseMyMind", logMessage.c_str());
#elif TARGET_PLATFORM ==  PLATFORM_WIN32
    std::wstring wstr = StringUtils::string2UTF8wstring(logMessage.c_str());
    OutputDebugStringW(wstr.c_str());
    printf(logMessage.c_str());
    fflush(stdout);
#else
    // Linux, Mac, iOS, etc
    fprintf(stdout, logMessage.c_str());
    fflush(stdout);
#endif
}




