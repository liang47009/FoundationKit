/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_DICTIONARY_HPP
#define FOUNDATIONKIT_DICTIONARY_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <unordered_map>
#include <vector>
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"

NS_FK_BEGIN

template < typename TKey, typename TValue >
class Dictionary
{
public:
    typedef typename std::unordered_multimap<TKey, TValue> dictionary;
    typedef typename dictionary::iterator iterator;
    typedef typename dictionary::const_iterator const_interator;
    typedef typename dictionary::key_type key_type;
    typedef typename dictionary::size_type size_type;
    typedef typename dictionary::value_type value_type;
    typedef typename dictionary::reference reference;
    typedef typename dictionary::const_reference const_reference;
    typedef typename dictionary::difference_type difference_type;
    typedef typename dictionary::allocator_type allocator_type;
    typedef typename dictionary::mapped_type mapped_type;
    typedef std::pair<TKey, size_t> ki_pair;
    typedef std::pair< typename dictionary::iterator, typename dictionary::iterator > dictionary_pair;

    Dictionary()
        : _MultiMap()
    {
    }

    Dictionary(const Dictionary & other)
        : _MultiMap(other._MultiMap)
    {
    }

    Dictionary(Dictionary && other)
        : _MultiMap(std::move(other._MultiMap))
    {
    }

    Dictionary(iterator begin, iterator end)
        : _MultiMap(begin, end)
    {
    }

    ~Dictionary()
    {
        clear();
    }
    
    Dictionary<TKey, TValue> & operator=(const Dictionary<TKey, TValue>& other)
    {
        _MultiMap = other._MultiMap;
        return (*this);
    }

    Dictionary<TKey, TValue> & operator=(Dictionary<TKey, TValue>&& other)
    {
        _MultiMap = std::move(other._MultiMap);
        return (*this);
    }

    TValue & operator[](const TKey & key)
    {
        return (at(key));
    }

    TValue & operator[](ki_pair & keyIndexPair)
    {
        return (at(keyIndexPair));
    }

    
    TValue & at(const TKey & key)
    {
        auto it = _MultiMap.find(key);
        LOG_ASSERT(it != _MultiMap.end(), "Dictionary::at: Couldn't find key:%s", StringUtils::Tostring(key).c_str());
        return ((*it).second);
    }

    TValue & at(const TKey & key, size_type index)
    {
        std::pair<iterator, iterator> range = _MultiMap.equal_range(key);
        auto it = range.first;
        size_type i(0);
        while (i < index && it != range.second)
        {
            ++i;
            ++it;
        }
        return ((*it).second);
        
    }

    TValue & at(ki_pair& keyIndexPair)
    {
        return (at(keyIndexPair.first, keyIndexPair.second));
    }
    
    std::vector<TValue> valuesOfKey(const TKey &key)
    {
        std::pair<iterator, iterator> range = _MultiMap.equal_range(key);
        std::vector<TValue>  values;
        for (auto it = range.first; it != range.second; ++it)
        {
            values.push_back(it->second);
        }
        return values;
    }

    std::vector<TKey> allKeys()
    {
        std::vector<TKey> keys;
        for (auto it : _MultiMap)
        {
            keys.push_back(it.first);
        }
        return keys;
    }

    iterator begin()
    {
        return (_MultiMap.begin());
    }

    iterator end()
    {
        return (_MultiMap.end());
    }
    
    const_interator cbegin() const
    {
        return (_MultiMap.cbegin());
    }

    const_interator cend() const
    {
        return (_MultiMap.cend());
    }
    
    const_interator find(const TKey & key) const
    {
        return (_MultiMap.find(key));
    }

    size_type count(const TKey & key) const
    {
        return (_MultiMap.count(key));
    }
    
    std::pair< const_interator, const_interator > equal_range(const TKey & key) const
    {
        return _MultiMap.equal_range(key);
    }

    const_interator lower_bound(const TKey & key) const
    {
        return (_MultiMap.lower_bound(key));
//      return _MultiMap.equal_range(key).first;
    }

    const_interator upper_bound(const TKey & key) const
    {
        return (_MultiMap.upper_bound(key));
//      return _MultiMap.equal_range(key).second;
    }
    
    iterator insert(const value_type & kv_pair)
    {
        return (_MultiMap.insert(kv_pair));
    }

    iterator insert(std::initializer_list<value_type> _Ilist)
    {
        return _MultiMap.insert(_Ilist);
    }

    iterator insert(const TKey& key, const TValue& value)
    {
        return _MultiMap.insert({ key, value });
    }
    
    iterator erase(const_interator position)
    {
        return (_MultiMap.erase(position));
    }

    size_type erase(const key_type& key)
    {
        return (_MultiMap.erase(key));
    }

    iterator erase(const_interator first, const_interator last)
    {
        return (_MultiMap.erase(first, last));
    }

    void clear()
    {
        _MultiMap.clear();
    }

    void swap(Dictionary<TKey, TValue> & refDict)
    {
        _MultiMap.swap(refDict._MultiMap);
    }

    bool empty() const
    {
        return (_MultiMap.empty());
    }

    size_type size() const
    {
        return (_MultiMap.size());
    }

    size_type max_size() const
    {
        return (_MultiMap.max_size());
    }
    
private:
    template < typename TFriendKey, typename TFriendValue >
    friend void swap( Dictionary<TFriendKey, TFriendValue>& lhs,
                     Dictionary<TFriendKey, TFriendValue>& rhs)
    {
        std::swap(lhs._MultiMap, rhs._MultiMap);
    }
    
    template < typename TFriendKey, typename TFriendValue >
    friend bool operator== ( Dictionary<TFriendKey, TFriendValue>& lhs,
                            Dictionary<TFriendKey, TValue>& rhs)
    {
        return (lhs._MultiMap == rhs._MultiMap);
    }
    
    template < typename TFriendKey, typename TFriendValue >
    friend bool operator!= ( Dictionary<TFriendKey, TFriendValue>& lhs,
                            Dictionary<TFriendKey, TValue>& rhs)
    {
        return (lhs._MultiMap != rhs._MultiMap);
    }
protected:
    dictionary _MultiMap;
};

NS_FK_END

#endif // FOUNDATIONKIT_DICTIONARY_HPP
