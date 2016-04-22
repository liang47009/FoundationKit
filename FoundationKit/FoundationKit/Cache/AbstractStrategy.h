/****************************************************************************
Definition of the AbstractCache class.

Copyright (c) 2006, Applied Informatics Software Engineering GmbH and Contributors.
Copyright (c) 2015-2016 libo.

losemymind.libo@gmail.com

Original file from pocoproject: https://github.com/pocoproject/poco

 This file was modified to fit the FoundationKit

****************************************************************************/


#ifndef LOSEMYMIND_ABSTRACTSTRATEGY_H
#define LOSEMYMIND_ABSTRACTSTRATEGY_H

#pragma once
#include "KeyValueArgs.h"
#include "FoundationKit/GenericPlatformMacros.h"
NS_FK_BEGIN

template <class TKey, class TValue>
// An AbstractStrategy is the interface for all strategies. 
class AbstractStrategy
{
public:
    AbstractStrategy()
    {
    }

    virtual ~AbstractStrategy()
    {
    }

    // Adds the key to the strategy.
    // If for the key already an entry exists, an exception will be thrown.
    virtual void onAdd(const KeyValueArgs <TKey, TValue>& key) = 0;

    // Updates an existing entry.
    virtual void onUpdate(const KeyValueArgs <TKey, TValue>& args)
    {
        onRemove(args.first);
        onAdd(args);
    }

    // Removes an entry from the strategy. If the entry is not found
    // the remove is ignored.
    virtual void onRemove(const TKey& key) = 0;

    // Informs the strategy that a read-access happens to an element.
    virtual void onGet(const TKey& key) = 0;

    // Removes all elements from the cache.
    virtual void onClear() = 0;

    // Used to query if a key is still valid (i.e. cached).
    virtual void onIsValid(ValidArgs<TKey>& key) = 0;

    // Used by the Strategy to indicate which elements should be removed from
    // the cache. Note that onReplace does not change the current list of keys.
    // The cache object is reponsible to remove the elements.
    virtual void onReplace(std::set<TKey>& elemsToRemove) = 0;

    virtual void onGetOrderKeys(std::list<TKey>& keys) = 0;

};

NS_FK_END
#endif // LOSEMYMIND_ABSTRACTSTRATEGY_H












