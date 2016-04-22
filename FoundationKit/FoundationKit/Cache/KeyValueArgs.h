/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#ifndef LOSEMYMIND_KEYVALUEARGS_H
#define LOSEMYMIND_KEYVALUEARGS_H

#pragma once

#include <utility>

#include "FoundationKit/GenericPlatformMacros.h"
NS_FK_BEGIN

template <class TKey, class TValue>
using KeyValueArgs = std::pair < TKey, TValue > ;

template <class TKey>
using ValidArgs = std::pair < TKey, bool > ;

NS_FK_END
#endif // LOSEMYMIND_KEYVALUEARGS_H





