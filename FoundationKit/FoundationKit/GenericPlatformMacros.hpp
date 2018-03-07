/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_GENERICPLATFORMMACROS_HPP
#define FOUNDATIONKIT_GENERICPLATFORMMACROS_HPP
#pragma once

#define FOUNDATIONKIT_VERSION_STRING "3.1.0"
#define FOUNDATIONKIT_PACKAGE_STRING "FoundationKit 3.1.0"
#define FOUNDATIONKIT_VERSION_MAJOR 3
#define FOUNDATIONKIT_VERSION_MINOR 1
#define FOUNDATIONKIT_VERSION_PATCH 0

// Version as a single hex number, e.g. 0x01000300 == 1.0.3
#define FOUNDATIONKIT_VERSION_HEX ((FOUNDATIONKIT_VERSION_MAJOR << 24) | \
                                   (FOUNDATIONKIT_VERSION_MINOR << 16) | \
                                   (FOUNDATIONKIT_VERSION_PATCH <<  8))

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
#define PLATFORM_LINUX 0
#define PLATFORM_ANDROID 0
#define PLATFORM_MAC 0
#define PLATFORM_IOS 0
#define PLATFORM_APPLE 0
#define PLATFORM_TVOS 0
#define PLATFORM_XBOXONE 0
#define PLATFORM_PS4 0
#define PLATFORM_HTML5 0
#define PLATFORM_HTML5_BROWSER 0
#define PLATFORM_HTML5_WIN32 0
#define PLATFORM_SWITCH 0
#define PLATFORM_FREEBSD 0
#define PLATFORM_WINRT 0

enum class EPlatform { Windows, Linux, Android, Mac, iOS, Apple,TVOS, XBoxOne,PS4,HTML5,Switch,FreeBSD, Unknow };

// WinRT target.
#if defined(__cplusplus_winrt)
#   include <winapifamily.h>
#   if WINAPI_FAMILY_ONE_PARTITION(WINAPI_FAMILY, WINAPI_PARTITION_APP)
#       undef  PLATFORM_WINRT
#   define PLATFORM_WINRT 1
#   endif // WINAPI_FAMILY_ONE_PARTITION(WINAPI_FAMILY, WINAPI_PARTITION_APP)
#endif

#if defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
// TARGET_OS_IPHONE inlcudes TARGET_OS_IOS TARGET_OS_TV and TARGET_OS_WATCH. see TargetConditionals.h
#   include <TargetConditionals.h>
#   include <AvailabilityMacros.h>
#   undef  PLATFORM_APPLE
#   define PLATFORM_APPLE 1
#endif

#if defined(TARGET_OS_IOS) && (TARGET_OS_IOS == 1)
#   undef  PLATFORM_IOS
#   define PLATFORM_IOS 1

#elif defined(__ANDROID__) || defined(ANDROID)
#   undef  PLATFORM_ANDROID
#   define PLATFORM_ANDROID 1

#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(WIN64) || defined(_WIN64)
#   undef  PLATFORM_WINDOWS
#   define PLATFORM_WINDOWS 1

#elif (defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)) && !defined(_CRAYC)
#   undef  PLATFORM_LINUX
#   define PLATFORM_LINUX 1

#elif(TARGET_OS_MAC)
#   undef  PLATFORM_MAC
#   define PLATFORM_MAC 1

#else
    #error Unknown platform
#endif

#if   PLATFORM_WINDOWS
constexpr EPlatform TargetPlatform = EPlatform::Windows;
#include "FoundationKit/Platform/windows/WindowsPlatform.hpp"

#elif PLATFORM_ANDROID
constexpr EPlatform TargetPlatform = EPlatform::Android;
#include "FoundationKit/Platform/Android/AndroidPlatform.hpp"

#elif PLATFORM_IOS
constexpr EPlatform TargetPlatform = EPlatform::iOS;
#include "FoundationKit/Platform/ios/IOSPlatform.hpp"

#elif PLATFORM_MAC
constexpr EPlatform TargetPlatform = EPlatform::Mac;
#include "FoundationKit/Platform/mac/MACPlatform.hpp"

#elif PLATFORM_LINUX
constexpr EPlatform TargetPlatform = EPlatform::Linux;
#include "FoundationKit/Platform/linux/LinuxPlatform.hpp"
#else
constexpr EPlatform TargetPlatform = EPlatform::Unknow;
#error Unknown platform
#endif

//if constexpr (TargetPlatform == EPlatform::Android)
//{
//}
//else if constexpr (TargetPlatform == EPlatform::iOS)
//{
//
//}

/**
 * Macro for marking up deprecated code, functions and types.
 *
 * Features that are marked as deprecated are scheduled to be removed from the code base
 * in a future release. If you are using a deprecated feature in your code, you should
 * replace it before upgrading to the next release. See the Upgrade Notes in the release
 * notes for the release in which the feature was marked deprecated.
 *
 * Sample usage (note the slightly different syntax for classes and structures):
 *
 *		DEPRECATED(4.6, "Message")
 *		void Function();
 *
 *		struct DEPRECATED(4.6, "Message") MODULE_API MyStruct
 *		{
 *			// StructImplementation
 *		};
 *		class DEPRECATED(4.6, "Message") MODULE_API MyClass
 *		{
 *			// ClassImplementation
 *		};
 *
 * @param VERSION The release number in which the feature was marked deprecated.
 * @param MESSAGE A message containing upgrade notes.
 */
 // Enable in C++14
#undef  DEPRECATED
#define DEPRECATED(VERSION, MESSAGE) [[deprecated(MESSAGE " Please update your code to the new API before upgrading to the next release, otherwise your project will no longer compile.")]]

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
#define _FILE_AND_LINE_ __FILE__ "(" STRINGIZE(__LINE__) ")"
//#define COMPILE_WARNING(MSG) __pragma(message( __FILE__ "(" STRINGIZE(__LINE__) "):Warning:" ##MSG))
//#define COMPILE_ERROR(MSG) __pragma(message( __FILE__ "(" STRINGIZE(__LINE__) "):Error:" ##MSG))

extern void __fail__(const char* expr, const char* file, int line);
extern void __log__(const char* fmt, ...);
#define FKLog(fmt, ...) __log__(fmt, ##__VA_ARGS__)
#define ASSERT_IF(CHECK, MSG)do{if((CHECK)){__fail__(MSG,__FILE__, __LINE__);}}while(false)

#if defined(_DEBUG) || defined(DEBUG)
    #define ASSERTED(CHECK, MSG)do{if(!(CHECK)){__fail__(MSG,__FILE__, __LINE__);}}while(false)
    #define ASSERTED_EXPRESSION(COND, EXPR) ((COND) ? (EXPR) : (__fail__(#COND, __FILE__, __LINE__), (EXPR)))
    #define DEBUG_MODE 1
    #define FKDebug(fmt, ...) __log__(fmt, ##__VA_ARGS__)
#else
    #define ASSERTED(CHECK, MSG) (CHECK)
    #define ASSERTED_EXPRESSION(COND, EXPR) (EXPR)
    #define DEBUG_MODE 0
    #define FKDebug(fmt, ...)
#endif


// IOS,ANDROID,MAC platform must be defined USE_FILE32API
//#ifndef USE_FILE32API
//#define USE_FILE32API 1
//#endif

#endif // #ifndef LOSEMYMIND_GENERICPLATFORMMACROS_H

