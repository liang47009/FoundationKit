/****************************************************************************
Copyright Kevlin Henney, 2000, 2001, 2002. All rights reserved.
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

Original file from boost: http://www.boost.org
                          https://github.com/boostorg/any/blob/master/include/boost/any.hpp
****************************************************************************/

#ifndef FoundationKit_any_H
#define FoundationKit_any_H
#if defined(_MSC_VER)
# pragma once
#endif
#include <algorithm>
#include <typeinfo>
#include "stddefines.hpp"
#include "utility.hpp"

_STD_BEGIN

class any
{
public:
    any() _NOEXCEPT
        : content(0)
    {
    }

    template<typename ValueType>
    any(const ValueType & value)
        : content(new holder <
        typename std::remove_cv<typename std::decay<const ValueType>::type>::type
        > (value))
    {
    }

    any(const any & other)
        : content(other.content ? other.content->clone() : 0)
    {
    }

    // Move constructor
    any(any&& other) _NOEXCEPT
        : content(other.content)
    {
        other.content = 0;
    }

    // Perfect forwarding of ValueType
    template<typename ValueType>
    any(ValueType&& value
        , typename std::disable_if<std::is_same<any&, ValueType>::value >::type* = 0 // disable if value has type `any&`
        , typename std::disable_if<std::is_const<ValueType>::value >::type* = 0) // disable if value has type `const ValueType&&`
        : content(new holder< typename std::decay<ValueType>::type >(static_cast<ValueType&&>(value)))
    {
    }


    ~any() _NOEXCEPT
    {
        delete content;
    }

public: // modifiers

    any & swap(any & rhs) _NOEXCEPT
    {
        std::swap(content, rhs.content);
        return *this;
    }

    any & operator=(const any& rhs)
    {
        any(rhs).swap(*this);
        return *this;
    }

    // move assignement
    any & operator=(any&& rhs) _NOEXCEPT
    {
        rhs.swap(*this);
        any().swap(rhs);
        return *this;
    }

    // Perfect forwarding of ValueType
    template <class ValueType>
    any & operator=(ValueType&& rhs)
    {
        any(static_cast<ValueType&&>(rhs)).swap(*this);
        return *this;
    }

public: // queries

    bool empty() const _NOEXCEPT
    {
        return !content;
    }

    void clear() _NOEXCEPT
    {
        any().swap(*this);
    }

    const std::type_info& type() const _NOEXCEPT
    {
        return content ? content->type() : typeid(void);
    }

//#ifndef BOOST_NO_MEMBER_TEMPLATE_FRIENDS
private: // types
//#else
//public: // types (public so any_cast can be non-friend)
//#endif

    class placeholder
    {
    public: // structors

        virtual ~placeholder()
        {
        }

    public: // queries

        virtual const std::type_info& type() const _NOEXCEPT = 0;

        virtual placeholder * clone() const = 0;

    };

    template<typename ValueType>
    class holder : public placeholder
    {
    public: // structors

        holder(const ValueType & value)
            : held(value)
        {
        }

        holder(ValueType&& value)
            : held(static_cast<ValueType&&>(value))
        {
        }
    public: // queries

        virtual const std::type_info& type() const _NOEXCEPT
        {
            return typeid(ValueType);
        }

            virtual placeholder * clone() const
        {
            return new holder(held);
        }

    public: // representation

        ValueType held;

    private: // intentionally left unimplemented
        holder & operator=(const holder &);
    };

//#ifndef BOOST_NO_MEMBER_TEMPLATE_FRIENDS

private: // representation

    template<typename ValueType>
    friend ValueType * any_cast(any *) _NOEXCEPT;

    template<typename ValueType>
    friend ValueType * unsafe_any_cast(any *) _NOEXCEPT;

//#else

//public: // representation (public so any_cast can be non-friend)

//#endif

    placeholder * content;

};

inline void swap(any & lhs, any & rhs) _NOEXCEPT
{
    lhs.swap(rhs);
}

class  bad_any_cast : public std::bad_cast

{
public:
    virtual const char * what() const _NOEXCEPT
    {
        return "bad_any_cast: failed conversion using any_cast";
    }
};

template<typename ValueType>
ValueType * any_cast(any * operand) _NOEXCEPT
{
    return operand && operand->type() == typeid(ValueType)
    ? &static_cast<any::holder<typename std::remove_cv<ValueType>::type> *>(operand->content)->held
    : 0;
}

template<typename ValueType>
inline const ValueType * any_cast(const any * operand) _NOEXCEPT
{
    return any_cast<ValueType>(const_cast<any *>(operand));
}

template<typename ValueType>
ValueType any_cast(any & operand)
{
    typedef typename remove_reference<ValueType>::type nonref;

    nonref * result = any_cast<nonref>(&operand);
    if (!result)
        throw(bad_any_cast());

    // Attempt to avoid construction of a temporary object in cases when 
    // `ValueType` is not a reference. Example:
    // `static_cast<std::string>(*result);` 
    // which is equal to `std::string(*result);`
    typedef typename std::conditional <
        std::is_reference<ValueType>::value,
        ValueType,
        typename std::add_lvalue_reference<ValueType>::type
    > ::type ref_type;

    return static_cast<ref_type>(*result);
}

template<typename ValueType>
inline ValueType any_cast(const any & operand)
{
    typedef typename std::remove_reference<ValueType>::type nonref;
    return any_cast<const nonref &>(const_cast<any &>(operand));
}


template<typename ValueType>
inline ValueType any_cast(any&& operand)
{
    static_assert(
        std::is_rvalue_reference<ValueType&&>::value /*true if ValueType is rvalue or just a value*/
        || std::is_const< typename std::remove_reference<ValueType>::type >::value,
        "any_cast shall not be used for getting nonconst references to temporary objects"
        );
    return any_cast<ValueType>(operand);
}



// Note: The "unsafe" versions of any_cast are not part of the
// public interface and may be removed at any time. They are
// required where we know what type is stored in the any and can't
// use typeid() comparison, e.g., when our types may travel across
// different shared libraries.
template<typename ValueType>
inline ValueType * unsafe_any_cast(any * operand) _NOEXCEPT
{
    return &static_cast<any::holder<ValueType> *>(operand->content)->held;
}

template<typename ValueType>
inline const ValueType* unsafe_any_cast(const any * operand) _NOEXCEPT
{
    return unsafe_any_cast<ValueType>(const_cast<any *>(operand));
}

_STD_END

#endif // FoundationKit_any_H


