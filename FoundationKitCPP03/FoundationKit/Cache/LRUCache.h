/****************************************************************************
Definition of the AbstractCache class.

Copyright (c) 2006, Applied Informatics Software Engineering GmbH and Contributors.
Copyright (c) 2015-2016 libo.

losemymind.libo@gmail.com

Original file from pocoproject: https://github.com/pocoproject/poco

 This file was modified to fit the FoundationKit

****************************************************************************/

#ifndef FOUNDATIONKIT_LRUCACHE_H
#define FOUNDATIONKIT_LRUCACHE_H

#pragma once

#include "AbstractCache.h"
#include "LRUStrategy.h"
#include "FoundationKit/GenericPlatformMacros.hpp"
NS_FK_BEGIN

template < typename TKey, typename TValue >
class LRUCache : public AbstractCache < TKey, TValue, LRUStrategy<TKey, TValue> >
    /// An LRUCache implements Least Recently Used caching. The default size for a cache is 1024 entries.
{
public:
    LRUCache(long size = 1024) :
        AbstractCache<TKey, TValue, LRUStrategy<TKey, TValue> >(LRUStrategy<TKey, TValue>(size))
    {
    }

    ~LRUCache()
    {
    }

private:
    LRUCache(const LRUCache& other) = delete;
    LRUCache(LRUCache&& other) = delete;
    LRUCache& operator = (const LRUCache& other) = delete;
    LRUCache& operator= (LRUCache&& other) = delete;
};

NS_FK_END
#endif // FOUNDATIONKIT_LRUCACHE_H






