/****************************************************************************
Copyright (c) 2016 libo All rights reserved.

losemymind.libo@gmail.com

****************************************************************************/
#ifndef LOSEMYMIND_DISABLE_IF_H
#define LOSEMYMIND_DISABLE_IF_H

#include <utility>
namespace std
{
/**
 * stl is not implement disable_if, so we implement it and add to std namespace.
 * written more simply as
 */
template < bool _Test, class _Ty = void > 
using disable_if = std::enable_if < !_Test, _Ty >;

} //namespace std

#endif // LOSEMYMIND_DISABLE_IF_H



