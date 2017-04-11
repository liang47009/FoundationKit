/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_KEYVALUEARGS_H
#define FOUNDATIONKIT_KEYVALUEARGS_H

#pragma once

#include <utility>

#include "FoundationKit/GenericPlatformMacros.hpp"
NS_FK_BEGIN

template <class TKey, class TValue>
using KeyValueArgs = std::pair < TKey, TValue > ;

template <class TKey>
using ValidArgs = std::pair < TKey, bool > ;

NS_FK_END
#endif // FOUNDATIONKIT_KEYVALUEARGS_H





