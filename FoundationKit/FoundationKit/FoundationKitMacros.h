
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


// VS2005: _MSC_VER=1400  (MSVC++ 8.0 )
// VS2008: _MSC_VER=1500  (MSVC++ 9.0 )
// VS2010: _MSC_VER=1600  (MSVC++ 10.0)
// VS2012: _MSC_VER=1700  (MSVC++ 11.0)
// VS2013: _MSC_VER=1800  (MSVC++ 12.0)
// VS2015: _MSC_VER=1900  (MSVC++ 14.0)

// https://sourceforge.net/p/predef/wiki/Standards/
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

#define UNUSED_PARAM(param) do{(void)(param);}while(0)

#ifdef __GNUC__
#define FK_UNUSED __attribute__ ((unused))
#else
#define FK_UNUSED
#endif

//#pragma message(__COMPILE_MSG__ "Show compile message")
#define __COMPILE_STR1__(s) #s
#define __COMPILE_STR2__(s) __COMPILE_STR1__(s)
#define __COMPILE_MSG__ __FILE__ "("__COMPILE_STR2__(__LINE__) "):Warning:" 


#endif // LOSEMYMIND_FOUNDATIONKITMACROS_H