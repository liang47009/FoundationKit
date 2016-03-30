
#ifndef LOSEMYMIND_FOUNDATIONKITMACROS_H
#define LOSEMYMIND_FOUNDATIONKITMACROS_H

#include <string> // for check _STD_BEGIN
#ifndef _STD_BEGIN

#if defined(__cplusplus)
#define _STD_BEGIN	namespace std {
#define _STD_END		}
#define _STD	::std::
#else /* __cplusplus */
#define _STD_BEGIN
#define _STD_END
#define _STD
#endif
#endif

#define CPP_VERSION_UNKNOWN 0
#define CPP_VERSION_98      1
#define CPP_VERSION_11      2
#define CPP_VERSION_14      3
#define CPP_VERSION_17      4
#define CPP_TARGET_VERSION  CPP_VERSION_UNKNOWN

#ifdef __cplusplus
#if __cplusplus == 199711L
#undef CPP_TARGET_VERSION
#define CPP_TARGET_VERSION CPP_VERSION_98
#endif

#if __cplusplus == 201103L || _MSC_VER > 1600
#undef CPP_TARGET_VERSION
#define CPP_TARGET_VERSION CPP_VERSION_11
#endif

#if __cplusplus == 201402L
#undef CPP_TARGET_VERSION
#define CPP_TARGET_VERSION CPP_VERSION_14
#endif

#endif //#ifdef __cplusplus

// check user set platform
#if !CPP_TARGET_VERSION
#error  "C++ Version is not support."
#endif 

#if (CPP_TARGET_VERSION < CPP_VERSION_14)
#define constexpr // constexpr support by c++14
#define noexcept  // noexcept support by c++14
#endif

/** @def __deprecated
* Only certain compilers support __attribute__((deprecated)).
*/
//#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#ifdef __GNUC__
    #ifndef __deprecated
    #define __deprecated     __attribute__((deprecated))
    #endif

    #ifndef __unused
    #define __unused         __attribute__((unused))
    #endif

    #ifndef __aligned
    #define __aligned(n)     __attribute__((aligned(n)))
    #endif

    #ifndef __align_max
    #define __align_max      __attribute__((aligned))
    #endif

    #ifndef __packed
    #define __packed         __attribute__((packed))
    #endif

#elif _MSC_VER
    #define __deprecated __declspec(deprecated)
    #define __unused
    #define __aligned(n)   __declspec(align(n))
    #define __align_max
    #define __packed

#else
    #define __deprecated
    #define __unused
    #define __align(n)
    #define __align_max
    #define __packed
#endif

#define __unused_arg(arg) do{(void)(arg);}while(0)

//
// Helper macro DO_STRINGIZE:
// Converts the parameter X to a string after macro replacement
// on X has been performed.
//
#define STRINGIZE_HELPER(X) #X
#define STRINGIZE(X) STRINGIZE_HELPER(X)

//#pragma message(COMPILE_MSG "Show compile message")
#define COMPILE_MSG __FILE__ "(" STRINGIZE(__LINE__) "):Warning:" 

#define TEXT_HELPER(a,b) a ## b
#define MAKE_TEXT(s) TEXT_HELPER(L, s)

#endif // LOSEMYMIND_FOUNDATIONKITMACROS_H