/****************************************************************************
Copyright (c) 2013-2014 libo.

losemymind.libo@gmail.com

****************************************************************************/
#pragma once

#include <unordered_map>
#include <vector>
#include "FoundationKit/FoundationKitDefines.h"

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

    Dictionary();
    Dictionary(const Dictionary & yRef);
    Dictionary(Dictionary && yRef);
    Dictionary(iterator begin, iterator end);
    ~Dictionary();
    
    Dictionary<TKey, TValue> & operator=(const Dictionary<TKey, TValue>& yRef);
    Dictionary<TKey, TValue> & operator=(Dictionary<TKey, TValue>&& yRef);
    TValue & operator[](const TKey & key);
    TValue & operator[](ki_pair & keyIndexPair);

    
    TValue & at(const TKey & key);
    TValue & at(const TKey & key, size_type index);
    TValue & at(ki_pair& keyIndexPair);
    
    std::vector<TValue> valuesOfKey(const TKey &key);
    std::vector<TKey> allKeys();

    iterator begin();
    iterator end();
    
    const_interator cbegin() const;
    const_interator cend() const;
    
    const_interator find(const TKey & key) const;
    size_type count(const TKey & key) const;
    
    std::pair< const_interator, const_interator >
    equal_range(const TKey & key) const;
    const_interator lower_bound(const TKey & key) const;
    const_interator upper_bound(const TKey & key) const;
    
    iterator insert(const value_type & kv_pair);
    iterator insert(std::initializer_list<value_type> _Ilist);
    
    iterator erase(const_interator position);
    size_type erase(const key_type& key);
    iterator erase(const_interator first, const_interator last);
    
    void clear();
    void swap(Dictionary<TKey, TValue> & yRef);
    bool empty() const;
    size_type size() const;
    size_type max_size() const;
    
protected:
    dictionary m_MultiMap;
    
private:
    template < typename TFriendKey, typename TFriendValue >
    friend void swap( Dictionary<TFriendKey, TFriendValue>& lhs,
                     Dictionary<TFriendKey, TFriendValue>& rhs);
    
    template < typename TFriendKey, typename TFriendValue >
    friend bool operator== ( Dictionary<TFriendKey, TFriendValue>& lhs,
                            Dictionary<TFriendKey, TValue>& rhs);
    
    template < typename TFriendKey, typename TFriendValue >
    friend bool operator!= ( Dictionary<TFriendKey, TFriendValue>& lhs,
                            Dictionary<TFriendKey, TValue>& rhs);
};

NS_FK_END

#include "Dictionary.inl"