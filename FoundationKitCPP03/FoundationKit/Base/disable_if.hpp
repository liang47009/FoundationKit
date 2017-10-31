/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_DISABLE_IF_HPP
#define FOUNDATIONKIT_DISABLE_IF_HPP

#include <utility>
#include "FoundationKit/GenericPlatformMacros.hpp"
NS_FK_BEGIN
/**
 * stl is not implement disable_if, so we implement it and add to std namespace.
 * written more simply as
 */
template < bool _Test, class _Ty = void > 
using disable_if = std::enable_if < !_Test, _Ty >;

NS_FK_END

#endif // FOUNDATIONKIT_DISABLE_IF_HPP
