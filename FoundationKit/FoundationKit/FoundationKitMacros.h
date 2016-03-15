
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

// constexpr support by c++14
#if (CPP_TARGET_VERSION < CPP_VERSION_14)
#define constexpr
#endif

#define UNUSED_PARAM(param) do{(void)(param);}while(0)

#ifdef __GNUC__
#define FK_UNUSED __attribute__ ((unused))
#else
#define FK_UNUSED
#endif


//
// Helper macro DO_STRINGIZE:
// Converts the parameter X to a string after macro replacement
// on X has been performed.
//
#define _DO_STRINGIZE(X) #X
#define DO_STRINGIZE(X) _DO_STRINGIZE(X)

//#pragma message(__COMPILE_MSG__ "Show compile message")
#define __COMPILE_MSG__ __FILE__ "("DO_STRINGIZE(__LINE__) "):Warning:" 





#endif // LOSEMYMIND_FOUNDATIONKITMACROS_H