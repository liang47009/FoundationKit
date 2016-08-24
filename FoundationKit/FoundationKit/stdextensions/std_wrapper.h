/****************************************************************************
 Copyright (c) 2016 libo All rights reserved.

 losemymind.libo@gmail.com

****************************************************************************/

#ifndef FOUNDATIONKIT_STD_WRAPPER_H
#define FOUNDATIONKIT_STD_WRAPPER_H

#include "stdconfig.h"
#include <string>

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

#if (CPP_TARGET_VERSION < CPP_VERSION_11)
typedef unsigned short char16_t;
typedef unsigned int char32_t;
_STD_BEGIN
typedef basic_string<char16_t, char_traits<char16_t>, allocator<char16_t> > u16string;
typedef basic_string<char32_t, char_traits<char32_t>, allocator<char32_t> > u32string;
_STD_END

#endif

#endif // #ifndef FOUNDATIONKIT_STD_WRAPPER_H