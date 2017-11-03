
#ifndef LOSEMYMIND_GENERICPLATFORMMACROS_H
#define LOSEMYMIND_GENERICPLATFORMMACROS_H
#pragma once

#include "LanguageFeatures.hpp"
// namespace FoundationKit {}
#ifdef __cplusplus
    #define NS_FK_BEGIN                     namespace FoundationKit {
    #define NS_FK_END                       }
    #define USING_NS_FK                     using namespace FoundationKit
    #define FK_NAME                         FoundationKit::
#else
    #define NS_FK_BEGIN 
    #define NS_FK_END 
    #define USING_NS_FK 
    #define FK_NAME  
#endif 


// see https://sourceforge.net/p/predef/wiki/Compilers/
// or boost\config\select_platform_config.hpp
// define supported target platform macro which to uses.
#define PLATFORM_WINDOWS 0
#define PLATFORM_XBOXONE 0
#define PLATFORM_APPLE 0
#define PLATFORM_MAC 0
#define PLATFORM_IOS 0
#define PLATFORM_TVOS 0
#define PLATFORM_ANDROID 0
#define PLATFORM_PS4 0
#define PLATFORM_HTML5 0
#define PLATFORM_HTML5_BROWSER 0
#define PLATFORM_HTML5_WIN32 0
#define PLATFORM_LINUX 0
#define PLATFORM_SWITCH 0
#define PLATFORM_FREEBSD 0

#if defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
// TARGET_OS_IPHONE inlcudes TARGET_OS_IOS TARGET_OS_TV and TARGET_OS_WATCH. see TargetConditionals.h
#include <TargetConditionals.h>
#include <AvailabilityMacros.h>
#undef  PLATFORM_APPLE
#define PLATFORM_APPLE 1
#endif

#if defined(TARGET_OS_IOS) && (TARGET_OS_IOS == 1)
#undef  PLATFORM_IOS
#define PLATFORM_IOS 1

#elif defined(__ANDROID__) || defined(ANDROID)
#undef  PLATFORM_ANDROID
#define PLATFORM_ANDROID 1

#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(WIN64) || defined(_WIN64)
#undef  PLATFORM_WINDOWS
#define PLATFORM_WINDOWS 1

#elif (defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)) && !defined(_CRAYC)
#undef  PLATFORM_LINUX
#define PLATFORM_LINUX 0

#elif(TARGET_OS_MAC)
#undef  PLATFORM_MAC
#define PLATFORM_MAC 1

#else
    #error Unknown platform
#endif

#if PLATFORM_WINDOWS
#include "FoundationKit/Platform/windows/WindowsPlatform.hpp"
#elif PLATFORM_ANDROID
#include "FoundationKit/Platform/Android/AndroidPlatform.hpp"
#elif PLATFORM_IOS
#include "FoundationKit/Platform/ios/IOSPlatform.hpp"
#elif PLATFORM_MAC
#include "FoundationKit/Platform/mac/MACPlatform.hpp"
#elif PLATFORM_LINUX
#include "FoundationKit/Platform/linux/LinuxPlatform.hpp"
#else
#error Unknown platform
#endif

// Enable in C++14
//#undef DEPRECATED
//#define DEPRECATED(VERSION, MESSAGE) [[deprecated(MESSAGE " Please update your code to the new API before upgrading to the next release, otherwise your project will no longer compile.")]]

#define UNUSED_ARG(arg)          do {(void)(arg);}while(0)
#define SAFE_DELETE(p)           do { if(p) { delete   (p); (p) = nullptr; } } while(0)
#define SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = nullptr; } } while(0)
#define SAFE_FREE(p)             do { if(p) { free(p); (p) = nullptr; } } while(0)
#define BREAK_IF(cond)           if(cond) break

// Make a string to Wide string
#define TEXT_CAT(a,b) a ## b
#define MAKE_TEXT(s) TEXT_CAT(L, s)

// Helper macro STRINGIZE:
// Converts the parameter X to a string after macro replacement
// on X has been performed.
#define STRINGIZE_HELPER(X) #X
#define STRINGIZE(X) STRINGIZE_HELPER(X)

//#pragma message(COMPILE_MSG "Show compile message")
#define COMPILE_MSG __FILE__ "(" STRINGIZE(__LINE__) "):Warning:" 

extern void __fail__(const char* expr, const char* file, int line);
extern void __log__(const char* fmt, ...);
#define FKLog(fmt, ...) __log__(fmt, ##__VA_ARGS__)
#if defined(_DEBUG) || defined(DEBUG)
    #define ASSERTED(CHECK, MSG)do{if(!(CHECK)){__fail__(#MSG,__FILE__, __LINE__);}}while(false)
    #define ASSERTED_EXPRESSION(COND, EXPR) ((COND) ? (EXPR) : (__fail__(#COND, __FILE__, __LINE__), (EXPR)))
    #define DEBUG_MODE 1
    #define FKDebug(fmt, ...) __log__(fmt, ##__VA_ARGS__)
#else
    #define ASSERTED(CHECK, MSG)
    #define ASSERTED_EXPRESSION(COND, EXPR) (EXPR)
    #define DEBUG_MODE 0
    #define FKDebug(fmt, ...)
#endif

//===============================================================================================
// Platform Pre-Setup
//===============================================================================================
#if PLATFORM_WINDOWS
__pragma (warning(disable:4127))
#pragma warning(disable:4127)
#define _XKEYCHECK_H // disable windows xkeycheck.h

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#ifndef _SCL_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS 1
#endif

/*
 * Include windows.h without Windows Sockets 1.1 to prevent conflicts with
 * Windows Sockets 2.0.
 */
#include <windows.h>

#endif //PLATFORM_WINDOWS

// IOS,ANDROID,MAC platform must be defined USE_FILE32API
//#ifndef USE_FILE32API
//#define USE_FILE32API 1
//#endif

#include "FoundationKit/std/stdheader.h"


#define FOUNDATIONKIT_VERSION_STRING "2.2.0"
#define FOUNDATIONKIT_PACKAGE_STRING "FoundationKit 2.2.0"
#define FOUNDATIONKIT_VERSION_MAJOR 2
#define FOUNDATIONKIT_VERSION_MINOR 2
#define FOUNDATIONKIT_VERSION_PATCH 0

// Version as a single hex number, e.g. 0x01000300 == 1.0.3
#define FOUNDATIONKIT_VERSION_HEX ((FOUNDATIONKIT_VERSION_MAJOR << 24) | \
                                   (FOUNDATIONKIT_VERSION_MINOR << 16) | \
                                   (FOUNDATIONKIT_VERSION_PATCH <<  8))

#endif // #ifndef LOSEMYMIND_GENERICPLATFORMMACROS_H

