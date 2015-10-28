/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#pragma once

#include <utility>

#include "FoundationKit/FoundationKitDefines.h"
NS_FK_BEGIN

template <class TKey, class TValue>
using KeyValueArgs = std::pair < TKey, TValue > ;

template <class TKey>
using ValidArgs = std::pair < TKey, bool > ;

NS_FK_END