/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/

#pragma once

// namespace FoundationKit {}
#ifdef __cplusplus
#define NS_FK_BEGIN                     namespace FoundationKit {
#define NS_FK_END                       }
#define USING_NS_FK                     using namespace FoundationKit
#define NS_NAME                         FoundationKit
#else
#define NS_FK_BEGIN 
#define NS_FK_END 
#define USING_NS_FK 
#define NS_NAME
#endif 



/************************************************************************/
/*              Config of Foundation, per target platform.              */
/************************************************************************/

// define supported target platform macro which FK uses.
#define FK_PLATFORM_UNKNOWN            0
#define FK_PLATFORM_IOS                1
#define FK_PLATFORM_ANDROID            2
#define FK_PLATFORM_WIN32              3
#define FK_PLATFORM_MARMALADE          4
#define FK_PLATFORM_LINUX              5
#define FK_PLATFORM_BADA               6
#define FK_PLATFORM_BLACKBERRY         7
#define FK_PLATFORM_MAC                8
#define FK_PLATFORM_NACL               9
#define FK_PLATFORM_EMSCRIPTEN        10
#define FK_PLATFORM_TIZEN             11
#define FK_PLATFORM_QT5               12
#define FK_PLATFORM_WP8               13
#define FK_PLATFORM_WINRT             14

// Determine target platform by compile environment macro.
#define FK_TARGET_PLATFORM         FK_PLATFORM_UNKNOWN

// mac
#if defined(FK_TARGET_OS_MAC)
#undef  FK_TARGET_PLATFORM
#define FK_TARGET_PLATFORM         FK_PLATFORM_MAC
#endif

// iphone
#if defined(FK_TARGET_OS_IPHONE)
#undef  FK_TARGET_PLATFORM
#define FK_TARGET_PLATFORM         FK_PLATFORM_IOS
#endif

// android
#if defined(ANDROID)
#undef  FK_TARGET_PLATFORM
#define FK_TARGET_PLATFORM         FK_PLATFORM_ANDROID
#endif

// win32
#if defined(WIN32) && defined(_WINDOWS)
#undef  FK_TARGET_PLATFORM
#define FK_TARGET_PLATFORM         FK_PLATFORM_WIN32
#endif

// linux
#if defined(LINUX)
#undef  FK_TARGET_PLATFORM
#define FK_TARGET_PLATFORM         FK_PLATFORM_LINUX
#endif

// marmalade
#if defined(MARMALADE)
#undef  FK_TARGET_PLATFORM
#define FK_TARGET_PLATFORM         FK_PLATFORM_MARMALADE
#endif

// bada
#if defined(SHP)
#undef  FK_TARGET_PLATFORM
#define FK_TARGET_PLATFORM         FK_PLATFORM_BADA
#endif

// qnx
#if defined(__QNX__)
#undef  FK_TARGET_PLATFORM
#define FK_TARGET_PLATFORM         FK_PLATFORM_BLACKBERRY
#endif

// native client
#if defined(__native_client__)
#undef  FK_TARGET_PLATFORM
#define FK_TARGET_PLATFORM         FK_PLATFORM_NACL
#endif

// Emscripten
#if defined(EMSCRIPTEN)
#undef  FK_TARGET_PLATFORM
#define FK_TARGET_PLATFORM         FK_PLATFORM_EMSCRIPTEN
#endif

// tizen
#if defined(TIZEN)
#undef  FK_TARGET_PLATFORM
#define FK_TARGET_PLATFORM         FK_PLATFORM_TIZEN
#endif

// qt5
#if defined(FK_TARGET_QT5)
#undef  FK_TARGET_PLATFORM
#define FK_TARGET_PLATFORM         FK_PLATFORM_QT5
#endif

// WinRT (Windows Store App)
#if defined(WINRT)
#undef  FK_TARGET_PLATFORM
#define FK_TARGET_PLATFORM		   FK_PLATFORM_WINRT
#endif

// WP8 (Windows Phone 8 App)
#if defined(WP8)
#undef  FK_TARGET_PLATFORM
#define FK_TARGET_PLATFORM		   FK_PLATFORM_WP8
#endif

// check user set platform
#if !FK_TARGET_PLATFORM
#error  "Cannot recognize the target platform; are you targeting an unsupported platform?"
#endif 

#if (FK_TARGET_PLATFORM == FK_PLATFORM_WIN32)
#ifndef __MINGW32__
#pragma warning (disable:4127) 
#endif 
#endif  // FK_PLATFORM_WIN32


/************************************************************************/
/*              Figure out the abi based on defined macros.             */
/************************************************************************/

#define FK_ABI_UNKNOWN    0
#define FK_ABI_ARM        1
#define FK_ABI_ARM64      2
#define FK_ABI_MIPS       3
#define FK_ABI_MIPS64     4
#define FK_ABI_X86        5
#define FK_ABI_X86_64     6

#define FK_TARGET_ABI     FK_ABI_UNKNOWN

#if defined(__arm__)
#define FK_ABI_STRING "arm"
#undef FK_TARGET_ABI
#define FK_TARGET_ABI     FK_ABI_ARM

#elif defined(__aarch64__)
#define FK_ABI_STRING "arm64"
#undef FK_TARGET_ABI
#define FK_TARGET_ABI     FK_ABI_ARM64

#elif defined(__mips__) && !defined(__LP64__)
#define FK_ABI_STRING "mips"
#undef FK_TARGET_ABI
#define FK_TARGET_ABI     FK_ABI_MIPS

#elif defined(__mips__) && defined(__LP64__)
#define FK_ABI_STRING "mips64"
#undef FK_TARGET_ABI
#define FK_TARGET_ABI     FK_ABI_MIPS64

#elif defined(__i386__) ||defined(_WINDOWS)
#define FK_ABI_STRING "x86"
#undef FK_TARGET_ABI
#define FK_TARGET_ABI     FK_ABI_X86

#elif defined(__x86_64__)
#define FK_ABI_STRING "x86_64"
#undef FK_TARGET_ABI
#define FK_TARGET_ABI     FK_ABI_X86_64

#else
#error "Unsupported ABI"
#endif





/************************************************************************/
/*              Finds the compiler type and version.                    */
/************************************************************************/
#define FK_COMPILER_MSVC 1
#define FK_COMPILER_GNUC 2
#define FK_COMPILER_INTEL 3
#define FK_COMPILER_CLANG 4

#if defined( _MSC_VER )
#   define FK_COMPILER FK_COMPILER_MSVC
#   define FK_COMP_VER _MSC_VER
#	define FK_THREADLOCAL __declspec(thread)

#elif defined( __GNUC__ )
#   define FK_COMPILER BS_COMPILER_GNUC
#   define FK_COMP_VER (((__GNUC__)*100) + \
    (__GNUC_MINOR__*10) + \
    __GNUC_PATCHLEVEL__)
#   define FK_THREADLOCAL __thread

#elif defined ( __INTEL_COMPILER )
#   define FK_COMPILER BS_COMPILER_INTEL
#	define FK_COMP_VER __INTEL_COMPILER
#if (FK_TARGET_PLATFORM == FK_PLATFORM_WIN32)
#   define FK_THREADLOCAL __declspec(thread)
#else
#   define FK_THREADLOCAL __thread
#endif

#elif defined ( __clang__ )
#   define FK_COMPILER BS_COMPILER_CLANG
#	define FK_COMP_VER __clang_major__
#   define FK_THREADLOCAL __thread
#else
#   pragma error "No known compiler. "

#endif

// Pre Debug Model
#if (FK_TARGET_PLATFORM == FK_PLATFORM_WIN32 || FK_TARGET_PLATFORM == FK_PLATFORM_WP8)
#   if defined(_DEBUG) || defined(DEBUG)
#       define FK_DEBUG
#   endif
#else
#   ifdef DEBUG
#       define FK_DEBUG
#   endif
#endif

extern void _log_(const char* message, ...);

#define FK_SAFE_DELETE(p)           do { delete (p); (p) = nullptr; } while(0)
#define FK_SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = nullptr; } } while(0)
#define FK_SAFE_FREE(p)             do { if(p) { free(p); (p) = nullptr; } } while(0)
#define FK_SAFE_RELEASE(p)          do { if(p) { (p)->release(); } } while(0)
#define FK_SAFE_RELEASE_NULL(p)     do { if(p) { (p)->release(); (p) = nullptr; } } while(0)
#define FK_SAFE_RETAIN(p)           do { if(p) { (p)->retain(); } } while(0)
#define FK_BREAK_IF(cond)           if(cond) break
#define FK_BREAK_IF_DBG(cond,msg) if(cond) {        \
                                        _log_(msg); \
                                        break;      \
                                        } 
#define FK_MAX_PATH 512
#define UNUSED_PARAM(param) do{(void)(param);}while(0)

#ifdef __GNUC__
#define FK_UNUSED __attribute__ ((unused))
#else
#define FK_UNUSED
#endif

#include <assert.h>
#ifdef FK_DEBUG
#define GP_ASSERT(cond) assert(cond)
#define FK_ASSERT(cond) assert(cond)
#define FKASSERT(cond, msg) do {              \
    if (!(cond)) {                            \
        _log_(msg);                           \
        assert(cond);                         \
    }                                         \
} while (0)
#else
#define FKASSERT(cond, msg)
#define GP_ASSERT(cond)
#define FK_ASSERT(cond) assert(cond)
#endif

//#pragma message(__COMPILE_MSG__ "Show compile message")
#define __COMPILE_STR1__(s) #s
#define __COMPILE_STR2__(s) __COMPILE_STR1__(s)
#define __COMPILE_MSG__ __FILE__ "("__COMPILE_STR2__(__LINE__) "):Warning:" 







