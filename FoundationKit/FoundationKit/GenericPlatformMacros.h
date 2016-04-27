
#ifndef LOSEMYMIND_GENERICPLATFORMMACROS_H
#define LOSEMYMIND_GENERICPLATFORMMACROS_H
#pragma once
#include <cassert>
// namespace FoundationKit {}
#ifdef __cplusplus
#define NS_FK_BEGIN                     namespace FoundationKit {
#define NS_FK_END                       }
#define USING_NS_FK                     using namespace FoundationKit
#else
#define NS_FK_BEGIN 
#define NS_FK_END 
#define USING_NS_FK 
#endif 


// https://sourceforge.net/p/predef/wiki/Compilers/
// define supported target platform macro which to uses.
#define PLATFORM_UNKNOWN            0
#define PLATFORM_IOS                1
#define PLATFORM_ANDROID            2
#define PLATFORM_WIN32              3
#define PLATFORM_LINUX              4
#define PLATFORM_MAC                5

#define TARGET_PLATFORM PLATFORM_UNKNOWN

#if defined(__APPLE__)
// TARGET_OS_IPHONE inlcudes TARGET_OS_IOS TARGET_OS_TV and TARGET_OS_WATCH. see TargetConditionals.h
#include <TargetConditionals.h>
#endif

#if (TARGET_OS_IOS )
    #undef TARGET_PLATFORM
    #define TARGET_PLATFORM PLATFORM_IOS
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
    #define DEPRECATED(VERSION, MESSAGE) __attribute__((deprecated(MESSAGE " Please update your code to the new API before upgrading to the next release, otherwise your project will no longer compile.")))
    // DLL export and import definitions
    #define DLLEXPORT
    #define DLLIMPORT
    // Alignment.
    #define ATTRIBUTE_PACK(n) __attribute__((packed,aligned(n)))
    #define ATTRIBUTE_ALIGN(n) __attribute__((aligned(n)))
    #define ATTRIBUTE_UNUSED __attribute__((unused))
    #define FORCEINLINE inline __attribute__ ((always_inline))	/* Force code to be inline */
    #define FORCENOINLINE __attribute__((noinline))	            /* Force code to NOT be inline */


#elif defined(__ANDROID__) || defined(ANDROID)
    #undef TARGET_PLATFORM
    #define TARGET_PLATFORM PLATFORM_ANDROID
    #define DEPRECATED(VERSION, MESSAGE) __attribute__((deprecated(MESSAGE " Please update your code to the new API before upgrading to the next release, otherwise your project will no longer compile.")))
    // DLL export and import definitions
    #define DLLEXPORT			__attribute__((visibility("default")))
    #define DLLIMPORT			__attribute__((visibility("default")))
    // Alignment.
    #define ATTRIBUTE_PACK(n) __attribute__((packed,aligned(n)))
    #define ATTRIBUTE_ALIGN(n) __attribute__((aligned(n)))
    #define ATTRIBUTE_UNUSED __attribute__((unused))
    #define FORCEINLINE inline __attribute__ ((always_inline)) /* Force code to be inline */
    #define FORCENOINLINE __attribute__((noinline))	           /* Force code to NOT be inline */


#elif defined(_WIN32) && defined(_WINDOWS)
    #undef TARGET_PLATFORM
    #define TARGET_PLATFORM PLATFORM_WIN32
    #define DEPRECATED(VERSION, MESSAGE) __declspec(deprecated(MESSAGE " Please update your code to the new API before upgrading to the next release, otherwise your project will no longer compile."))
    // DLL export and import definitions
    #define DLLEXPORT __declspec(dllexport)
    #define DLLIMPORT __declspec(dllimport)
    // Alignment.
    #define ATTRIBUTE_PACK(n)
    #define ATTRIBUTE_ALIGN(n) __declspec(align(n))
    #define ATTRIBUTE_UNUSED
    #define FORCEINLINE __forceinline				  /* Force code to be inline */
    #define FORCENOINLINE __declspec(noinline)        /* Force code to NOT be inline */	

#elif (defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)) && !defined(_CRAYC)
    #undef TARGET_PLATFORM    
    #define TARGET_PLATFORM PLATFORM_LINUX
    #define DEPRECATED(VERSION, MESSAGE) __attribute__((deprecated(MESSAGE " Please update your code to the new API before upgrading to the next release, otherwise your project will no longer compile.")))
    // DLL export and import definitions
    #define DLLEXPORT			__attribute__((visibility("default")))
    #define DLLIMPORT			__attribute__((visibility("default")))
    // Alignment.
    #define ATTRIBUTE_PACK(n) __attribute__((packed,aligned(n)))
    #define ATTRIBUTE_ALIGN(n) __attribute__((aligned(n)))
    #define ATTRIBUTE_UNUSED __attribute__((unused))
    #define FORCEINLINE inline __attribute__ ((always_inline)) /* Force code to be inline */
    #define FORCENOINLINE __attribute__((noinline))	           /* Force code to NOT be inline */

#elif(TARGET_OS_MAC)
    #undef TARGET_PLATFORM
    #define TARGET_PLATFORM PLATFORM_MAC
    #define DEPRECATED(VERSION, MESSAGE) __attribute__((deprecated(MESSAGE " Please update your code to the new API before upgrading to the next release, otherwise your project will no longer compile.")))
    // DLL export and import definitions
    #define DLLEXPORT
    #define DLLIMPORT
    // Alignment.
    #define ATTRIBUTE_PACK(n) __attribute__((packed,aligned(n)))
    #define ATTRIBUTE_ALIGN(n) __attribute__((aligned(n)))
    #define ATTRIBUTE_UNUSED __attribute__((unused))
    #define FORCEINLINE inline __attribute__ ((always_inline))  /* Force code to be inline */
    #define FORCENOINLINE __attribute__((noinline))	            /* Force code to NOT be inline */

#else
    #error Unknown platform
#endif

#if (TARGET_PLATFORM == PLATFORM_WIN32)
__pragma (warning(disable:4127))
#define _XKEYCHECK_H // disable windows xkeycheck.h
#endif

extern void __log__(const char* message, ...);
#if defined(_DEBUG) || defined(DEBUG)
#define LOG_ASSERT(cond, msg,...) do{if (!(cond)){ __log__(msg, ##__VA_ARGS__);assert(cond);}} while (false)
#define DEBUG_MODE 1

#else
#define LOG_ASSERT(cond, msg) do{}while(false)
#define DEBUG_MODE 0

#endif

#if __cplusplus < 201103L
#define constexpr
#define noexcept
#endif

#define __unused_arg(arg) do{(void)(arg);}while(0)

#define SAFE_DELETE(p)           do { delete (p); (p) = nullptr; } while(0)
#define SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = nullptr; } } while(0)
#define SAFE_FREE(p)             do { if(p) { free(p); (p) = nullptr; } } while(0)
#define SAFE_RELEASE(p)          do { if(p) { (p)->release(); } } while(0)
#define SAFE_RELEASE_NULL(p)     do { if(p) { (p)->release(); (p) = nullptr; } } while(0)
#define SAFE_RETAIN(p)           do { if(p) { (p)->retain(); } } while(0)
#define BREAK_IF(cond)           if(cond) break

// Make a string to Wide string
#define TEXT_HELPER(a,b) a ## b
#define MAKE_TEXT(s) TEXT_HELPER(L, s)

// Helper macro STRINGIZE:
// Converts the parameter X to a string after macro replacement
// on X has been performed.
#define STRINGIZE_HELPER(X) #X
#define STRINGIZE(X) STRINGIZE_HELPER(X)

//#pragma message(COMPILE_MSG "Show compile message")
#define COMPILE_MSG __FILE__ "(" STRINGIZE(__LINE__) "):Warning:" 

/// when define returns true it means that our architecture uses big endian
#define HOST_IS_BIG_ENDIAN (bool)(*(unsigned short *)"\0\xff" < 0x100) 
#define ENDIAN_SWAP32(i)  ((i & 0x000000ff) << 24 | (i & 0x0000ff00) << 8 | (i & 0x00ff0000) >> 8 | (i & 0xff000000) >> 24)
#define ENDIAN_SWAP16(i)  ((i & 0x00ff) << 8 | (i &0xff00) >> 8)   
#define SWAP_INT32_LITTLE_TO_HOST(i) ((HOST_IS_BIG_ENDIAN == true)? ENDIAN_SWAP32(i) : (i) )
#define SWAP_INT16_LITTLE_TO_HOST(i) ((HOST_IS_BIG_ENDIAN == true)? ENDIAN_SWAP16(i) : (i) )
#define SWAP_INT32_BIG_TO_HOST(i)    ((HOST_IS_BIG_ENDIAN == true)? (i) : ENDIAN_SWAP32(i) )
#define SWAP_INT16_BIG_TO_HOST(i)    ((HOST_IS_BIG_ENDIAN == true)? (i):  ENDIAN_SWAP16(i) )



#endif // #ifndef LOSEMYMIND_GENERICPLATFORMMACROS_H

