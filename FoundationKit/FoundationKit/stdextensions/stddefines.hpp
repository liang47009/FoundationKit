/****************************************************************************
 Copyright (c) 2015 libo All rights reserved.
 
 losemymind.libo@gmail.com
 
 ****************************************************************************/


#pragma once

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

#if __cplusplus < 201103L
#ifdef _MSC_VER
//#  define noexcept throw ()
//#  define noexcept(x) throw ()
#endif
#endif