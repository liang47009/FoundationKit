//------------------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   xlDef.h
//    Author:      Streamlet
//    Create Time: 2010-10-14
//    Description: 
//
//    Version history:
//
//
//
//------------------------------------------------------------------------------

#ifndef __XLDEF_H_1C304B99_7B3A_4BDB_9BE7_0C76AD3C1126_INCLUDED__
#define __XLDEF_H_1C304B99_7B3A_4BDB_9BE7_0C76AD3C1126_INCLUDED__

#include "FoundationKit/stdextensions/stdconfig.h"

#if ENABLE_STD_EXTENSIONS
#define ENABLE_XL_LIBRARY
#endif

#ifdef ENABLE_XL_LIBRARY


namespace xl
{

#if defined(_MSC_VER)
#    define XL_SIZE_T_DEFINED
#  if _MSC_VER >= 1600
#    define XL_NULLPTR_DEFINED
#  endif
#elif defined(__GNUC__)
#  if !defined(size_t)
typedef unsigned int size_t;
#  endif
#endif




} // namespace xl

#endif // #ifdef ENABLE_XL_LIBRARY

#endif // #ifndef __XLDEF_H_1C304B99_7B3A_4BDB_9BE7_0C76AD3C1126_INCLUDED__
