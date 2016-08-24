/****************************************************************************
 Copyright (c) 2016 libo All rights reserved.

 losemymind.libo@gmail.com

****************************************************************************/
#ifndef APPLICATIONKIT_STDEXTENSIONS_STDCONFIG_H
#define APPLICATIONKIT_STDEXTENSIONS_STDCONFIG_H

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

#if(!defined(_MSC_VER))
#undef CPP_TARGET_VERSION
#define CPP_TARGET_VERSION CPP_VERSION_11
#endif

#endif //#ifdef __cplusplus

// check user set platform
#if !CPP_TARGET_VERSION
#error  "C++ Version is not support."
#endif 

#if (CPP_TARGET_VERSION < CPP_VERSION_11)
#define ENABLE_STD_EXTENSIONS 1
#else
#define ENABLE_STD_EXTENSIONS 0
#endif

#ifndef nullptr
#define nullptr 0
#endif

#endif