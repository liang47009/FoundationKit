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
void __log__(const char* message, ...)
{
    static const size_t MAX_LOG_LENGTH = 1024;
    va_list args;

    // Declare a moderately sized buffer on the stack that should be
    // large enough to accommodate most log requests.
    int size = MAX_LOG_LENGTH;
    //char stackBuffer[MAX_LOG_LENGTH] = { 0 };
    std::vector<char> dynamicBuffer(MAX_LOG_LENGTH);
    char* str = &dynamicBuffer[0];
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


#if (TARGET_PLATFORM == PLATFORM_ANDROID)
    __android_log_print(ANDROID_LOG_INFO, "LoseMyMind", str);
#elif TARGET_PLATFORM ==  PLATFORM_WIN32
    std::wstring wstr = StringUtils::string2UTF8wstring(str);
    OutputDebugStringW(wstr.c_str());
    printf(str);
    fflush(stdout);
#else
    // Linux, Mac, iOS, etc
    fprintf(stdout, str);
    fflush(stdout);
#endif
}




