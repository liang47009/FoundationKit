

#include <cstdarg>
#include <cstdio>
#include <cassert>
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationMacros.hpp"
#if (TARGET_PLATFORM == PLATFORM_ANDROID)
#include <android/log.h>
#elif (TARGET_PLATFORM == PLATFORM_WINDOWS)
#include <Windows.h>
#endif
USING_NS_FK;

void __fail__(const char* expr, const char* file, int line)
{
# if defined(EMSCRIPTEN) && EMSCRIPTEN
    __assert_fail(expr, file, line, "");
# elif defined __native_client__
    __assert(expr, line, file); // WHY.
# elif defined __ANDROID__
    __assert(file, line, expr);
# elif defined __clang__ || defined __GNU_LIBRARY__ || (defined __GNUC__ && defined __APPLE__)
    __assert(expr, file, line);
# elif defined __GNUC__
    _assert(expr, file, line);
# elif defined _MSC_VER
    #if DEBUG_MODE
    _CrtDbgReport(_CRT_ASSERT, file, line, file, expr);
    #else
    UNUSED_ARG(file);
    UNUSED_ARG(line);
    UNUSED_ARG(expr);
    #endif // DEBUG
# else
#   error UNSUPPORTED COMPILER
# endif
}

const size_t MAX_LOG_LENGTH = 256;
void __log__(const char* fmt, ...)
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

