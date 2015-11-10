/****************************************************************************
Copyright (c) 2013-2014 libo.

losemymind.libo@gmail.com

****************************************************************************/
#include <unordered_map>
#include "FoundationKit/Foundation/Logger.h"

NS_FK_BEGIN

template < typename TKey, typename TValue >
Dictionary<TKey, TValue>::Dictionary()
: m_MultiMap()
{
    
}

template < typename TKey, typename TValue >
Dictionary<TKey, TValue>::Dictionary(const Dictionary & yRef)
: m_MultiMap(yRef.m_MultiMap)
{
    
}

template < typename TKey, typename TValue >
Dictionary<TKey, TValue>::Dictionary(Dictionary && yRef)
: m_MultiMap(yRef.m_MultiMap)
{
}

template < typename TKey, typename TValue >
Dictionary<TKey, TValue>::Dictionary
(	typename Dictionary<TKey, TValue>::iterator begin,
 typename Dictionary<TKey, TValue>::iterator end )
: m_MultiMap(begin, end)
{
    
}

template < typename TKey, typename TValue >
Dictionary<TKey, TValue>::~Dictionary()
{
    clear();
}

template < typename TKey, typename TValue >
Dictionary<TKey, TValue> &
Dictionary<TKey, TValue>::operator=(const Dictionary<TKey, TValue>& yRef)
{
    m_MultiMap = yRef.m_MultiMap;
    return (*this);
}

template < typename TKey, typename TValue >
Dictionary<TKey, TValue> &
Dictionary<TKey, TValue>::operator=(Dictionary<TKey, TValue>&& yRef)
{
    m_MultiMap = yRef.m_MultiMap;
    return (*this);
}

template < typename TKey, typename TValue >
TValue & Dictionary<TKey, TValue>::operator[](const TKey & key)
{
    return (at(key));
}

template < typename TKey, typename TValue >
TValue & Dictionary<TKey, TValue>::operator[](ki_pair& keyIndexPair)
{
    return (at(keyIndexPair));
}

template < typename TKey, typename TValue >
TValue & Dictionary<TKey, TValue>::at(const TKey & key)
{
    auto it = m_MultiMap.find(key);
    FKASSERT(it != m_MultiMap.end(), "Dictionary::at: Couldn't find key '");
    return ((*it).second);
}

template < typename TKey, typename TValue >
TValue & Dictionary<TKey, TValue>::at(ki_pair& keyIndexPair)
{
    return (at(keyIndexPair.first, keyIndexPair.second));
}

template < typename TKey, typename TValue >
TValue & Dictionary<TKey, TValue>::at(const TKey & key, size_type index)
{
    std::pair<iterator, iterator> range = m_MultiMap.equal_range(key);
    auto it = range.first;
    size_type i(0);
    while(i < index && it != range.second)
    {
        ++i;
        ++it;
        FKASSERT(it != range.second, "Dictionary::at: The value with key '");
    }
    return ((*it).second);
}

template < typename TKey, typename TValue >
std::vector<TValue> Dictionary<TKey, TValue>::valuesOfKey(const TKey &key)
{
    std::pair<iterator, iterator> range = m_MultiMap.equal_range(key);
    std::vector<TValue>  values;
    for (auto it = range.first; it != range.second; ++it)
    {
        values.push_back(it->second);
    }
    return values;
}

template < typename TKey, typename TValue >
std::vector<TKey> Dictionary<TKey, TValue>::allKeys()
{
    std::vector<TKey> keys;
    for (auto it : m_MultiMap)
    {
        keys.push_back(it.first);
    }
    return keys;
}

template < typename TKey, typename TValue >
typename Dictionary<TKey, TValue>::iterator Dictionary<TKey, TValue>::begin()
{
    return (m_MultiMap.begin());
}

template < typename TKey, typename TValue >
typename Dictionary<TKey, TValue>::iterator Dictionary<TKey, TValue>::end()
{
    return (m_MultiMap.end());
}

template < typename TKey, typename TValue >
typename Dictionary<TKey, TValue>::const_interator Dictionary<TKey, TValue>::cbegin() const
{
    return (m_MultiMap.cbegin());
}

template < typename TKey, typename TValue >
typename Dictionary<TKey, TValue>::const_interator Dictionary<TKey, TValue>::cend() const
{
    return (m_MultiMap.cend());
}

template < typename TKey, typename TValue >
typename Dictionary<TKey, TValue>::const_interator Dictionary<TKey, TValue>::find(const TKey & key) const
{
    return (m_MultiMap.find(key));
}

template < typename TKey, typename TValue >
typename Dictionary<TKey, TValue>::size_type Dictionary<TKey, TValue>::count(const TKey & key) const
{
    return (m_MultiMap.count(key));
}

template < typename TKey, typename TValue >
std::pair<
typename Dictionary<TKey, TValue>::const_interator,
typename Dictionary<TKey, TValue>::const_interator
>
Dictionary<TKey, TValue>::equal_range(const TKey & key) const
{
    return m_MultiMap.equal_range(key);
}

template < typename TKey, typename TValue >
typename Dictionary<TKey, TValue>::const_interator
Dictionary<TKey, TValue>::lower_bound(const TKey & key) const
{
#ifdef DESKTOP
    return (m_MultiMap.lower_bound(key));
#else
    return m_MultiMap.equal_range(key).first;
#endif
}

template < typename TKey, typename TValue >
typename Dictionary<TKey, TValue>::const_interator
Dictionary<TKey, TValue>::upper_bound(const TKey & key) const
{
#ifdef DESKTOP
    return (m_MultiMap.upper_bound(key));
#else
    return m_MultiMap.equal_range(key).second;
#endif
}

template < typename TKey, typename TValue >
typename Dictionary<TKey, TValue>::iterator
Dictionary<TKey, TValue>::insert(const value_type & kv_pair)
{
    return (m_MultiMap.insert(kv_pair));
}


template < typename TKey, typename TValue >
typename Dictionary<TKey, TValue>::iterator
Dictionary<TKey, TValue>::insert(std::initializer_list<value_type> _Ilist)
{
    return m_MultiMap.insert(_Ilist);
}

template < typename TKey, typename TValue >
typename Dictionary<TKey, TValue>::iterator
Dictionary<TKey, TValue>::insert(const TKey& key, const TValue& value)
{
    return m_MultiMap.insert({key, value});
}

template < typename TKey, typename TValue >
typename Dictionary<TKey, TValue>::iterator
Dictionary<TKey, TValue>::erase
(typename Dictionary<TKey, TValue>::const_interator position)
{
    return (m_MultiMap.erase(position));
}

template < typename TKey, typename TValue >
typename Dictionary<TKey, TValue>::size_type
Dictionary<TKey, TValue>::erase
(const typename Dictionary<TKey, TValue>::key_type& key)
{
    return (m_MultiMap.erase(key));
}

template < typename TKey, typename TValue >
typename Dictionary<TKey, TValue>::iterator
Dictionary<TKey, TValue>::erase(
                                typename Dictionary<TKey, TValue>::const_interator first,
                                typename Dictionary<TKey, TValue>::const_interator last )
{
    return (m_MultiMap.erase(first, last));
}

template < typename TKey, typename TValue >
void Dictionary<TKey, TValue>::clear()
{
    m_MultiMap.clear();
}

template < typename TKey, typename TValue >
void Dictionary<TKey, TValue>::swap(Dictionary<TKey, TValue> & yRef)
{
    m_MultiMap.swap(yRef.m_MultiMap);
}

template < typename TKey, typename TValue >
bool Dictionary<TKey, TValue>::empty() const
{
    return (m_MultiMap.empty());
}

template < typename TKey, typename TValue >
typename Dictionary<TKey, TValue>::size_type
Dictionary<TKey, TValue>::size() const
{
    return (m_MultiMap.size());
}

template < typename TKey, typename TValue >
typename Dictionary<TKey, TValue>::size_type
Dictionary<TKey, TValue>::max_size() const
{
    return (m_MultiMap.max_size());
}

template < typename TKey, typename TValue >
void swap( Dictionary<TKey, TValue>& lhs,
          Dictionary<TKey, TValue>& rhs)
{
    std::swap(lhs.m_MultiMap, rhs.m_MultiMap);
}

template < typename TKey, typename TValue >
bool operator== ( Dictionary<TKey, TValue>& lhs,
                 Dictionary<TKey, TValue>& rhs)
{
    return (lhs.m_MultiMap == rhs.m_MultiMap);
}

template < typename TKey, typename TValue >
bool operator!= ( Dictionary<TKey, TValue>& lhs,
                 Dictionary<TKey, TValue>& rhs)
{
    return (lhs.m_MultiMap != rhs.m_MultiMap);
}

NS_FK_END

