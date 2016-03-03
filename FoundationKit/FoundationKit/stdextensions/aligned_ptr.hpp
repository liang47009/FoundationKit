/****************************************************************************
Copyright (c) 2015 libo All rights reserved.

losemymind.libo@gmail.com

****************************************************************************/

#ifndef FoundationKit_memory_H
#define FoundationKit_memory_H
#if defined(_MSC_VER)
# pragma once
#endif

#include <memory>
#include "std_wrapper.h"
#include "utility.hpp"

_STD_BEGIN

/**
 * @code
     struct MyStruct
     {
        int a;
        double b;
     };
     memory_aligned<MyStruct>::type _pMyStruct;
     new(&_pMyStruct)MyStruct();
     MyStruct *_pRawMyStruct = reinterpret_cast<MyStruct*>(&_pMyStruct);
     _pRawMyStruct-> a = _pRawMyStruct->b *10;

     _pRawMyStruct->~MyStruct();

 * @endcode
 */
template<typename T>
struct memory_aligned
{
    using type = typename _STD aligned_storage<sizeof(T), _STD alignment_of<T>::value>::type;
};


/**
 * @code
    aligned_ptr<Value> _pMyStruct1;
    {
        aligned_ptr<Value> _pMyStruct;
        _pMyStruct = "asjflsdjfldsjf";
        _pMyStruct1 = _pMyStruct;
        Value  val(_pMyStruct == _pMyStruct1);
    }
    aligned_ptr<Value> _pMyStruct2 = std::move(_pMyStruct1);
 * @endcode
 */

template<class _Ty>
class aligned_ptr
{
    template<class _OTy>
    struct aligned_ptr_holder
    {
        aligned_ptr_holder() :_Ptr(nullptr){}
        ~aligned_ptr_holder()
        {
            _Ptr->~_OTy();
        }
        typedef typename memory_aligned<_OTy>::type  _AlignedType;
        _OTy*         _Ptr;
        _AlignedType _AlignedPtr;
    };
public:
    typedef aligned_ptr<_Ty>   _Myt;
    typedef aligned_ptr_holder<_Ty>  _HolderType;
    typedef _STD shared_ptr<_HolderType> _ContextType;

    template<typename... Args>
    aligned_ptr(Args&&... args) _NOEXCEPT
    {
        _content = _ContextType(new _HolderType());
        new(&_content->_AlignedPtr)_Ty(std::forward<Args>(args)...);
        _content->_Ptr = reinterpret_cast<_Ty*>(&_content->_AlignedPtr);
    }

    aligned_ptr(const _Myt& _Other)
    {
        _content = _Other._content;
    }

    aligned_ptr(_Myt&& _Other)
    {
        _content = _STD move(_Other._content);

    }
    ~aligned_ptr() _NOEXCEPT
    {
    }

    _Myt& operator=(_Myt&& _Right) _NOEXCEPT
    {
        aligned_ptr(_STD move(_Right)).swap(*this);
        return (*this);
    }

     template<class _Ty2>
    _Myt& operator=(aligned_ptr<_Ty2>&& _Right) _NOEXCEPT
    {
        aligned_ptr(_STD move(_Right)).swap(*this);
        return (*this);
    }

    _Myt& operator=(const _Myt& _Right) _NOEXCEPT
    {
        aligned_ptr(_Right).swap(*this);
        return (*this);
    }

     template<class _Ty2>
    _Myt& operator=(const aligned_ptr<_Ty2>& _Right) _NOEXCEPT
    {	
        aligned_ptr(_Right).swap(*this);
        return (*this);
    }

    typename std::add_lvalue_reference<_Ty>::type operator*() const _NOEXCEPT
    {	// return reference to resource
        return get();
    }

    _Ty *operator->() const _NOEXCEPT
    {	// return pointer to resource
        return get();
    }

    explicit operator bool() const _NOEXCEPT
    {	// test if aligned_ptr object owns no resource
        return (get() != 0);
    }

    _Ty *get() const _NOEXCEPT
    {	// return pointer to resource
        return _content->_Ptr;
    }

    void swap(_Myt& _Other) _NOEXCEPT
    {	
        std::swap(_content, _Other._content);
    }

    void reset() _NOEXCEPT
    {	// release resource and convert to empty aligned_ptr object
        aligned_ptr().swap(*this);
    }

private:
    _ContextType _content;
};

template<class _Ty1,class _Ty2>
bool operator==(const aligned_ptr<_Ty1>& _Left, const aligned_ptr<_Ty2>& _Right) _NOEXCEPT
{
    return (_Left.get() == _Right.get());
}

template<class _Ty1,class _Ty2>
bool operator!=(const aligned_ptr<_Ty1>& _Left, const aligned_ptr<_Ty2>& _Right) _NOEXCEPT
{	// test if aligned_ptr != aligned_ptr
    return (!(_Left == _Right));
}

template<class _Ty1,class _Ty2>
bool operator<(const aligned_ptr<_Ty1>& _Left, const aligned_ptr<_Ty2>& _Right) _NOEXCEPT
{	// test if aligned_ptr < aligned_ptr
    return (less<decltype(always_false<_Ty1>::value ? _Left.get() : _Right.get())>()(  _Left.get(), _Right.get()));
}

template<class _Ty1, class _Ty2>
bool operator>=(const aligned_ptr<_Ty1>& _Left, const aligned_ptr<_Ty2>& _Right) _NOEXCEPT
{	// aligned_ptr >= aligned_ptr
    return (!(_Left < _Right));
}

template<class _Ty1, class _Ty2>
bool operator>(const aligned_ptr<_Ty1>& _Left, const aligned_ptr<_Ty2>& _Right) _NOEXCEPT
{	// test if aligned_ptr > aligned_ptr
    return (_Right < _Left);
}

template<class _Ty1, class _Ty2>
bool operator<=(const aligned_ptr<_Ty1>& _Left, const aligned_ptr<_Ty2>& _Right) _NOEXCEPT
{	// test if aligned_ptr <= aligned_ptr
    return (!(_Right < _Left));
}

template<class _Ty>
bool operator==(const aligned_ptr<_Ty>& _Left, nullptr_t) _NOEXCEPT
{	// test if aligned_ptr == nullptr
    return (_Left.get() == (_Ty *)0);
}

template<class _Ty>
bool operator==(nullptr_t, const aligned_ptr<_Ty>& _Right) _NOEXCEPT
{	// test if nullptr == aligned_ptr
    return ((_Ty *)0 == _Right.get());
}

template<class _Ty>
bool operator!=(const aligned_ptr<_Ty>& _Left, nullptr_t _Right) _NOEXCEPT
{	// test if aligned_ptr != nullptr
    return (!(_Left == _Right));
}

template<class _Ty>
bool operator!=(nullptr_t _Left, const aligned_ptr<_Ty>& _Right) _NOEXCEPT
{	// test if nullptr != aligned_ptr
    return (!(_Left == _Right));
}

template<class _Ty>
bool operator<(const aligned_ptr<_Ty>& _Left, nullptr_t _Right) _NOEXCEPT
{	// test if aligned_ptr < nullptr
    return (less<_Ty *>()(_Left.get(), (_Ty *)0));
}

template<class _Ty>
bool operator<(nullptr_t _Left, const aligned_ptr<_Ty>& _Right) _NOEXCEPT
{	// test if nullptr < aligned_ptr
    return (less<_Ty *>()((_Ty *)0, _Right.get()));
}

template<class _Ty>
bool operator>=(const aligned_ptr<_Ty>& _Left, nullptr_t _Right) _NOEXCEPT
{	// test if aligned_ptr >= nullptr
    return (!(_Left < _Right));
}

template<class _Ty>
bool operator>=(nullptr_t _Left, const aligned_ptr<_Ty>& _Right) _NOEXCEPT
{	// test if nullptr >= aligned_ptr
    return (!(_Left < _Right));
}

template<class _Ty>
bool operator>(const aligned_ptr<_Ty>& _Left, nullptr_t _Right) _NOEXCEPT
{	// test if aligned_ptr > nullptr
    return (_Right < _Left);
}

template<class _Ty>
bool operator>(nullptr_t _Left, const aligned_ptr<_Ty>& _Right) _NOEXCEPT
{	// test if nullptr > aligned_ptr
    return (_Right < _Left);
}

template<class _Ty>
bool operator<=(const aligned_ptr<_Ty>& _Left, nullptr_t _Right) _NOEXCEPT
{	// test if aligned_ptr <= nullptr
    return (!(_Right < _Left));
}

template<class _Ty>
bool operator<=(nullptr_t _Left, const aligned_ptr<_Ty>& _Right) _NOEXCEPT
{	// test if nullptr <= aligned_ptr
    return (!(_Right < _Left));
}

template<class _Elem,class _Traits,class _Ty>
_STD basic_ostream<_Elem, _Traits>& operator<<(_STD basic_ostream<_Elem, _Traits>& _Out, const aligned_ptr<_Ty>& _Px)
{	// write contained pointer to stream
    return (_Out << _Px.get());
}

_STD_END

#endif // #ifndef FoundationKit_memory_H


