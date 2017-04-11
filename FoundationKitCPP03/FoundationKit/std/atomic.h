/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_ATOMIC_H
#define FOUNDATIONKIT_ATOMIC_H



#include "mutex.h"

namespace std
{

template<class _Ty>
struct atomic
{
    typedef _Ty _ITYPE;

    operator _Ty()const   //ÒþÊ½×ª»»º¯Êý
    {
        std::lock_guard<std::mutex> lock(_My_mutex);
        return _My_val;
    }

    _ITYPE operator=(_ITYPE val) volatile _NOEXCEPT
    {
        std::lock_guard<std::mutex> lock(_My_mutex);
        _My_val = val;
        return _My_val;

    }
    _ITYPE operator=(_ITYPE val) _NOEXCEPT
    {
        std::lock_guard<std::mutex> lock(_My_mutex);
        _My_val = val;
        return _My_val;
    }
    _ITYPE operator++(int val) volatile _NOEXCEPT
    {
        std::lock_guard<std::mutex> lock(_My_mutex);
        _My_val++;
        return _My_val;
    }
    _ITYPE operator++(int val) _NOEXCEPT
    {
        std::lock_guard<std::mutex> lock(_My_mutex);
        _My_val++;
        return _My_val;
    }
    _ITYPE operator--(int val) volatile _NOEXCEPT
    {
        std::lock_guard<std::mutex> lock(_My_mutex);
        _My_val--;
        return _My_val;
    }
    _ITYPE operator--(int val) _NOEXCEPT
    {
        std::lock_guard<std::mutex> lock(_My_mutex);
        _My_val--;
        return _My_val;
    }
    _ITYPE operator++() volatile _NOEXCEPT
    {
        std::lock_guard<std::mutex> lock(_My_mutex);
        ++_My_val;
        return _My_val;
    }
    _ITYPE operator++() _NOEXCEPT
    {
        std::lock_guard<std::mutex> lock(_My_mutex);
        ++_My_val;
        return _My_val;
    }
    _ITYPE operator--() volatile _NOEXCEPT
    {
        std::lock_guard<std::mutex> lock(_My_mutex);
        --_My_val;
        return _My_val;
    }
    _ITYPE operator--() _NOEXCEPT
    {
        std::lock_guard<std::mutex> lock(_My_mutex);
        --_My_val;
        return _My_val;
    }
    _ITYPE operator+=(_ITYPE val) volatile _NOEXCEPT
    {
        std::lock_guard<std::mutex> lock(_My_mutex);
        _My_val+= val;
        return _My_val;
    }
    _ITYPE operator+=(_ITYPE val) _NOEXCEPT
    {
        std::lock_guard<std::mutex> lock(_My_mutex);
        _My_val+= val;
        return _My_val;
    }
    _ITYPE operator-=(_ITYPE val) volatile _NOEXCEPT
    {
        std::lock_guard<std::mutex> lock(_My_mutex);
        _My_val-= val;
        return _My_val;
    }
    _ITYPE operator-=(_ITYPE val) _NOEXCEPT
    {
        std::lock_guard<std::mutex> lock(_My_mutex);
        _My_val-= val;
        return _My_val;
    }
    _ITYPE operator&=(_ITYPE val) volatile _NOEXCEPT
    {
        std::lock_guard<std::mutex> lock(_My_mutex);
        _My_val&= val;
        return _My_val;
    }
    _ITYPE operator&=(_ITYPE val) _NOEXCEPT
    {
        std::lock_guard<std::mutex> lock(_My_mutex);
        _My_val&= val;
        return _My_val;
    }
    _ITYPE operator|=(_ITYPE val) volatile _NOEXCEPT
    {
        std::lock_guard<std::mutex> lock(_My_mutex);
        _My_val|= val;
        return _My_val;
    }
    _ITYPE operator|=(_ITYPE val) _NOEXCEPT
    {
        std::lock_guard<std::mutex> lock(_My_mutex);
        _My_val|= val;
        return _My_val;
    }
    _ITYPE operator^=(_ITYPE val) volatile _NOEXCEPT
    {
        std::lock_guard<std::mutex> lock(_My_mutex);
        _My_val^= val;
        return _My_val;
    }
    _ITYPE operator^=(_ITYPE val) _NOEXCEPT
    {
        std::lock_guard<std::mutex> lock(_My_mutex);
        _My_val^= val;
        return _My_val;
    }
    //_ITYPE operator+=(ptrdiff_t val) volatile _NOEXCEPT
    //{
    //    std::lock_guard lock(_My_mutex);
    //    _My_val+= val;
    //    return _My_val;
    //}
    //_ITYPE operator+=(ptrdiff_t val) _NOEXCEPT
    //{
    //    std::lock_guard lock(_My_mutex);
    //    _My_val+= val;
    //    return _My_val;
    //}
    //_ITYPE operator-=(ptrdiff_t val) volatile _NOEXCEPT
    //{
    //    std::lock_guard lock(_My_mutex);
    //    _My_val-= val;
    //    return _My_val;
    //}
    //_ITYPE operator-=(ptrdiff_t val) _NOEXCEPT
    //{
    //    std::lock_guard lock(_My_mutex);
    //    _My_val-= val;
    //    return _My_val;
    //}

private:
    _ITYPE _My_val;
    mutex  _My_mutex;
};
}//namespace std
#endif // FOUNDATIONKIT_ATOMIC_H