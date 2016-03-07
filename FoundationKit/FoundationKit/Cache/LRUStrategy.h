/****************************************************************************
Definition of the AbstractCache class.

Copyright (c) 2006, Applied Informatics Software Engineering GmbH and Contributors.
Copyright (c) 2015-2016 libo.

losemymind.libo@gmail.com

Original file from pocoproject: https://github.com/pocoproject/poco

 This file was modified to fit the FoundationKit

****************************************************************************/

#ifndef LOSEMYMIND_LRUSTRATEGY_H
#define LOSEMYMIND_LRUSTRATEGY_H

#pragma once
#include <list>
#include <unordered_map>
#include "AbstractStrategy.h"
#include "FoundationKit/FoundationKitDefines.h"
NS_FK_BEGIN

// An LRUStrategy implements least recently used cache replacement.
template <class TKey, class TValue>
class LRUStrategy : public AbstractStrategy < TKey, TValue >
{
public:
    typedef std::list<TKey>                    Keys;
    typedef typename Keys::iterator            Iterator;
    typedef typename Keys::const_iterator      ConstIterator;
    typedef std::unordered_map<TKey, Iterator> KeyIndex;
    typedef typename KeyIndex::iterator        IndexIterator;
    typedef typename KeyIndex::const_iterator  ConstIndexIterator;

public:
    LRUStrategy(std::size_t size)
        : _size(size)
    {
        if (_size < 1) throw std::invalid_argument("size must be > 0");
    }

    ~LRUStrategy()
    {
    }

    virtual void onAdd(const KeyValueArgs <TKey, TValue>& args) final
    {
        _keys.push_front(args.first);
        std::pair<IndexIterator, bool> stat = _keyIndex.insert(std::make_pair(args.first, _keys.begin()));
        if (!stat.second)
        {
            stat.first->second = _keys.begin();
        }
    }

    virtual void onRemove(const TKey& key) final
    {
        IndexIterator it = _keyIndex.find(key);

        if (it != _keyIndex.end())
        {
            _keys.erase(it->second);
            _keyIndex.erase(it);
        }
    }

    virtual void onGet(const TKey& key) final
    {
        // LRU: in case of an hit, move to begin
        IndexIterator it = _keyIndex.find(key);

        if (it != _keyIndex.end())
        {
            _keys.splice(_keys.begin(), _keys, it->second); //_keys.erase(it->second)+_keys.push_front(key);
            it->second = _keys.begin();
        }
    }

    virtual void onClear() final
    {
        _keys.clear();
        _keyIndex.clear();
    }

    virtual void onIsValid(ValidArgs<TKey>& args) final
    {
        if (_keyIndex.find(args.first) == _keyIndex.end())
        {
            args.second = false;
        }
    }

    virtual void onReplace(std::set<TKey>& elemsToRemove) final
    {
        // Note: replace only informs the cache which elements
        // it would like to remove!
        // it does not remove them on its own!
        std::size_t curSize = _keyIndex.size();

        if (curSize < _size)
        {
            return;
        }

        std::size_t diff = curSize - _size;
        Iterator it = --_keys.end(); //--keys can never be invoked on an empty list due to the minSize==1 requirement of LRU
        std::size_t i = 0;

        while (i++ < diff)
        {
            elemsToRemove.insert(*it);
            if (it != _keys.begin())
            {
                --it;
            }
        }
    }

    virtual void onGetOrderKeys(Keys& keys) final
    {
        keys = _keys;
    }

protected:
    std::size_t _size;     /// Number of keys the cache can store.
    Keys        _keys;
    KeyIndex    _keyIndex; /// For faster access to _keys
};

NS_FK_END
#endif // LOSEMYMIND_LRUSTRATEGY_H







