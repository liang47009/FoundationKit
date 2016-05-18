/****************************************************************************
Definition of the AbstractCache class.

Copyright (c) 2006, Applied Informatics Software Engineering GmbH and Contributors.
Copyright (c) 2015-2016 libo.

losemymind.libo@gmail.com

Original file from pocoproject: https://github.com/pocoproject/poco

 This file was modified to fit the FoundationKit

****************************************************************************/

#ifndef LOSEMYMIND_ABSTRACTCACHE_H
#define LOSEMYMIND_ABSTRACTCACHE_H

#pragma once
#include <unordered_map>
#include <map>
#include <set>
#include <mutex>
#include <memory>
#include <iostream>
#include <list>
#include "KeyValueArgs.h"
#include "FoundationKit/GenericPlatformMacros.h"
NS_FK_BEGIN

template<typename TKey, typename TValue, typename TStrategy>
class AbstractCache
{
public:
    typedef std::shared_ptr<TValue>             ValuePtr;
    typedef std::unordered_map<TKey, ValuePtr>  DataHolder;
    typedef typename DataHolder::iterator       Iterator;
    typedef typename DataHolder::const_iterator ConstIterator;
    typedef std::list<TKey>                     KeyList;


    AbstractCache()
    {
        initialize();
    }
    AbstractCache(const TStrategy& strat) : _strategy(strat)
    {
        initialize();
    }

    virtual ~AbstractCache()
    {

    }

    // Adds the key value pair to the cache.
    // If for the key already an entry exists, it will be overwritten.
    void add(const TKey& key, const TValue& val)
    {
        std::lock_guard<std::mutex>  lock(_mutex);
        doAdd(key, val);
    }

    // Adds the key value pair to the cache. Note that adding a nullptr std::shared_ptr will fail!
    // If for the key already an entry exists, it will be overwritten, ie. first a remove event
    // is thrown, then a add event
    void add(const TKey& key, std::shared_ptr<TValue > val)
    {
        std::lock_guard<std::mutex>  lock(_mutex);
        doAdd(key, val);
    }

    /// Adds the key value pair to the cache. Note that adding a NULL SharedPtr will fail!
    /// If for the key already an entry exists, it will be overwritten.
    /// The difference to add is that no remove or add events are thrown in this case, 
    /// just a simply silent update is performed
    /// If the key doesnot exist the behavior is equal to add, ie. an add event is thrown
    void update(const TKey& key, const TValue& val)
    {
        std::lock_guard<std::mutex>  lock(_mutex);
        doUpdate(key, val);
    }

    /// Adds the key value pair to the cache. Note that adding a NULL SharedPtr will fail!
    /// If for the key already an entry exists, it will be overwritten.
    /// The difference to add is that no remove or add events are thrown in this case, 
    /// just an Update is thrown
    /// If the key doesnot exist the behavior is equal to add, ie. an add event is thrown
    void update(const TKey& key, std::shared_ptr<TValue > val)
    {
        std::lock_guard<std::mutex>  lock(_mutex);
        doUpdate(key, val);
    }

    /// Removes an entry from the cache. If the entry is not found,
    /// the remove is ignored.
    void remove(const TKey& key)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        Iterator it = _data.find(key);
        doRemove(it);
    }

    /// Returns true if the cache contains a value for the key.
    bool has(const TKey& key) const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return doHas(key);
    }

    /// Returns a SharedPtr of the value. The SharedPointer will remain valid
    /// even when cache replacement removes the element.
    /// If for the key no value exists, an empty SharedPtr is returned.
    std::shared_ptr<TValue> get(const TKey& key)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return doGet(key);
    }

    /// Removes all elements from the cache.
    void clear()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        doClear();
    }

    std::size_t size()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        doReplace();
        return _data.size();
    }

    /// Forces cache replacement. Note that Poco's cache strategy use for efficiency reason no background thread
    /// which periodically triggers cache replacement. Cache Replacement is only started when the cache is modified
    /// from outside, i.e. add is called, or when a user tries to access an cache element via get.
    /// In some cases, i.e. expire based caching where for a long time no access to the cache happens,
    /// it might be desirable to be able to trigger cache replacement manually.
    void forceReplace()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        doReplace();
    }

    KeyList getAllKeys(bool bRaw = true)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        doReplace();
        KeyList result;

        if (bRaw)
        {
            ConstIterator it = _data.begin();
            ConstIterator itEnd = _data.end();
            for (; it != itEnd; ++it)
                result.push_back(it->first);
        }
        else
        {
            GetOrderKeysFun(result);
        }
        return result;
    }

    void dumpDebug()
    {
        KeyList keys = getAllKeys(false);
        std::lock_guard<std::mutex> lock(_mutex);
        std::cout << "Cache Size:" << keys.size() << std::endl;
        for (auto& key : keys)
        {
            std::cout << "{" << key << ":" << *(_data[key]) << "}" << std::endl;
        }
    }

protected:
    std::function < void(const KeyValueArgs <TKey, TValue>&) > AddFun;
    std::function < void(const KeyValueArgs <TKey, TValue>&) > UpdateFun;
    std::function < void(const TKey&) >                        RemoveFun;
    std::function < void(const TKey&) >                        GetFun;
    std::function < void() >                                   ClearFun;
    std::function < void(ValidArgs<TKey>&)>                    IsValidFun;
    std::function < void(std::set<TKey>&)>                     ReplaceFun;
    std::function < void(std::list<TKey>&)>                    GetOrderKeysFun;

    void initialize()
    {
        AddFun          = std::bind(&TStrategy::onAdd,     &_strategy, std::placeholders::_1);
        UpdateFun       = std::bind(&TStrategy::onUpdate,  &_strategy, std::placeholders::_1);
        RemoveFun       = std::bind(&TStrategy::onRemove,  &_strategy, std::placeholders::_1);
        GetFun          = std::bind(&TStrategy::onGet,     &_strategy, std::placeholders::_1);
        ClearFun        = std::bind(&TStrategy::onClear,   &_strategy);
        IsValidFun      = std::bind(&TStrategy::onIsValid, &_strategy, std::placeholders::_1);
        ReplaceFun      = std::bind(&TStrategy::onReplace, &_strategy, std::placeholders::_1);
        GetOrderKeysFun = std::bind(&TStrategy::onGetOrderKeys, &_strategy, std::placeholders::_1);
    }

    /// Adds the key value pair to the cache.
    /// If for the key already an entry exists, it will be overwritten.
    void doAdd(const TKey& key, const TValue& val)
    {
        doAdd(key, std::shared_ptr<TValue>(new TValue(val)));
    }

    /// Adds the key value pair to the cache.
    /// If for the key already an entry exists, it will be overwritten.
    void doAdd(const TKey& key, std::shared_ptr<TValue>& val)
    {
        Iterator it = _data.find(key);
        doRemove(it);
        KeyValueArgs<TKey, TValue> args(key, *val);
        AddFun(args);
        _data.insert(std::make_pair(key, val));
        doReplace();
    }

    /// Adds the key value pair to the cache.
    /// If for the key already an entry exists, it will be overwritten.
    void doUpdate(const TKey& key, const TValue& val)
    {
        doUpdate(key, std::shared_ptr<TValue>(new TValue(val)));
    }

    /// Adds the key value pair to the cache.
    /// If for the key already an entry exists, it will be overwritten.
    void doUpdate(const TKey& key, std::shared_ptr<TValue>& val)
    {
        KeyValueArgs<TKey, TValue> args(key, *val);
        Iterator it = _data.find(key);
        if (it == _data.end())
        {
            AddFun(args);
            _data.insert(std::make_pair(key, val));
        }
        else
        {
            UpdateFun(args);
            it->second = val;
        }
        doReplace();
    }

    /// Removes an entry from the cache. If the entry is not found
    /// the remove is ignored.
    void doRemove(Iterator it)
    {
        if (it != _data.end())
        {
            RemoveFun(it->first);
            _data.erase(it);
        }
    }

    /// Returns true if the cache contains a value for the key
    bool doHas(const TKey& key) const
    {
        ConstIterator it = _data.find(key);
        bool result = false;

        if (it != _data.end())
        {
            ValidArgs<TKey> args(key, true);
            IsValidFun(args);
            result = args.second;
        }

        return result;
    }

    std::shared_ptr<TValue> doGet(const TKey& key)
    {
        Iterator it = _data.find(key);
        std::shared_ptr<TValue> result;

        if (it != _data.end())
        {
            // inform all strategies that a read-access to an element happens
            GetFun(key);
            // ask all strategies if the key is valid
            ValidArgs<TKey> args(key, true);
            IsValidFun(args);

            if (!args.second)
            {
                doRemove(it);
            }
            else
            {
                result = it->second;
            }
        }

        return result;
    }

    void doClear()
    {
        ClearFun();
        _data.clear();
    }

    void doReplace()
    {
        std::set<TKey> delMe;
        ReplaceFun(delMe);
        // delMe contains the to be removed elements
        typename std::set<TKey>::const_iterator it = delMe.begin();
        typename std::set<TKey>::const_iterator endIt = delMe.end();

        for (; it != endIt; ++it)
        {
            Iterator itH = _data.find(*it);
            doRemove(itH);
        }
    }

private:
    TStrategy          _strategy;
    mutable DataHolder _data;
    mutable std::mutex _mutex;

};

NS_FK_END
#endif // LOSEMYMIND_ABSTRACTCACHE_H





