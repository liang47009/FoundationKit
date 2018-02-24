

#include <cstdarg>
#include <cstdio>
#include <cassert>
#include <string>
#include <vector>
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationMacros.hpp"
#if (PLATFORM_ANDROID)
#include <android/log.h>
#elif (PLATFORM_WINDOWS)
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

const size_t DEFAULT_LOG_LENGTH = 256;
void __log__(const char* fmt, ...)
{
    // Pass one greater needed size to leave room for NULL terminator
    std::vector<char> dynamicBuffer;
    char* result = nullptr;
    int BufferSize = DEFAULT_LOG_LENGTH;
    int needed = 0;
    int loopCount = 0;
    va_list arglist;
    do
    {
        // Pass one greater needed size to leave room for NULL terminator
        dynamicBuffer.resize(BufferSize + 1);
        result = &dynamicBuffer[0];
        /*
        pitfall: The behavior of vsnprintf between VS2013 and VS2015/2017 is different
        VS2013 or Unix-Like System will return -1 when buffer not enough, but VS2015/2017
        will return the actural needed length for buffer at this station
        The _vsnprintf behavior is compatible API which always return -1 when buffer isn't
        enough at VS2013/2015/2017 Yes, The vsnprintf is more efficient implemented by MSVC 19.0 or later, AND it's also standard-compliant, see reference: http://www.cplusplus.com/reference/cstdio/vsnprintf/
        */
        va_start(arglist, fmt);
        // Pass one greater needed size to leave room for NULL terminator
        needed = vsnprintf(result, BufferSize + 1, fmt, arglist);
        va_end(arglist);
        if (needed >= 0 && needed < BufferSize)
        {
            break;
        }
        else
        {
            BufferSize *= 2;
        }
    } while (++loopCount < 10);
    
    std::string strPreMsg = result;
    strPreMsg += "\n";


#if (PLATFORM_ANDROID)
    __android_log_print(ANDROID_LOG_INFO, "FoundationKit", "%s", strPreMsg.c_str());
#elif  PLATFORM_WINDOWS
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



