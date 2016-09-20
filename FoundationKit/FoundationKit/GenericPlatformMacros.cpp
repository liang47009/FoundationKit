

#include <cstdarg>
#include <cstdio>
#include <cassert>
#include "FoundationKit/Foundation/StringUtils.h"
#include "GenericPlatformMacros.h"
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
        _CrtDbgReport(_CRT_ASSERT, file, line, expr, "");
    #else
    UNUSED_ARG(file);
    UNUSED_ARG(line);
    UNUSED_ARG(expr);
    #endif // DEBUG
# else
#   error UNSUPPORTED COMPILER
# endif
}



