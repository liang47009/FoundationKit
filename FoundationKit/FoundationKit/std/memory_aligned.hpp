/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_MEMORY_ALIGNED_HPP
#define FOUNDATIONKIT_MEMORY_ALIGNED_HPP

#pragma once
#include <utility>
#include <type_traits>
#include <cassert>
#include <memory>
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Base/Types.hpp"
#include "FoundationKit/std/max_integer_of.hpp"
#include "FoundationKit/Foundation/Logger.hpp"

USING_NS_FK;

namespace std
{

template<typename T>
using memory_aligned_t = typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type;

// Max memory align value
const std::size_t alignment_of_max_align = std::alignment_of<std::max_align_t>::value;

/**
 * Get memory aligned size
 * @code
 *   size_t len = max_aligned<int, std::string, double, char[4], long long, long long int>::value;
 * @endcode
 * len is 8
 */
template<typename... Args>
struct max_aligned : std::integral_constant < int, max_integer_of<std::alignment_of<Args>::value...>::value > {};

/**
 * Aligns a value to the nearest higher multiple of 'Alignment', which must be a power of two.
 * @param Ptr			Value to align
 * @param Alignment		alignment, must be a power of two
 * @return				Aligned value
 */
template <typename T>
inline constexpr T* align(const T* Ptr, int32_t alignment)
{
    return (T*)((address(Ptr) + alignment - 1) & ~(alignment - 1));
}

/**
 * Checks if a value is alignment.
 * @param value - The value to check.
 * @return true if the value is alignment, false otherwise.
 */
constexpr inline bool is_alignment(std::size_t value) noexcept
{
    return (value > 0) && ((value & (value - 1)) == 0);
}

/**
 * Checks if a pointer is aligned to the specified alignment.
 * @param Ptr - The pointer to check.
 * @return true if the pointer is aligned, false otherwise.
 */
 inline bool is_alignment(const /*volatile*/ void* ptr, std::size_t alignment)noexcept
{
    ASSERTED(is_alignment(alignment),"alignment must be is ");
    return (address(ptr) & (alignment - 1)) == 0;
}

template< typename _Ty >
class aligned_ptr final 
{
    std::shared_ptr<memory_aligned_t<_Ty> > context;
public:
    typedef _Ty		         value_type;
    typedef _Ty&		     reference;
    typedef _Ty* 		     pointer;
    typedef const _Ty& 	     const_reference;
    typedef	const _Ty*       const_pointer;
    typedef const _Ty* const const_pointer_const;

    template< typename ...Args >
    explicit aligned_ptr(Args &&...args) 
    {
        void* dataPtr = ::operator new(sizeof(memory_aligned_t<_Ty>));
        new (dataPtr)value_type(std::forward< Args >(args)...);
        context.reset<memory_aligned_t<_Ty> >(reinterpret_cast<memory_aligned_t<_Ty>*>(dataPtr), 
        [](memory_aligned_t<_Ty>* ptr)
        {
            reinterpret_cast<pointer>(ptr)->~value_type();
            ::operator delete(ptr);
        });
    }

    ~aligned_ptr() 
    {

    }

    aligned_ptr(aligned_ptr &&other) noexcept 
        :context(std::move(other.context))
    {
    }
   
    aligned_ptr& operator=(aligned_ptr &&other) noexcept
    {
        //aligned_ptr(std::move(other)).swap(*this);
        //std::swap(context, other.context);
        context = std::move(other.context)
        return *this;
    }

    aligned_ptr(const aligned_ptr& other)
    {
        context = other.context;
    }

    aligned_ptr& operator=(const aligned_ptr&)
    {
        context = other.context;
        return *this;
    }

    reference operator*() 
    {
        return *reinterpret_cast<pointer>(context.get());
    }

    const_reference operator*() const 
    {
        return *reinterpret_cast<const_pointer>(context.get());
    }

    pointer operator->() 
    {
        return reinterpret_cast<pointer>(context.get());
    }

    const_pointer operator->() const 
    {
        return reinterpret_cast<const_pointer>(context.get());
    }

    pointer get() 
    {
        return reinterpret_cast<pointer>(context.get());
    }

    const_pointer get() const 
    {
        return reinterpret_cast<const_pointer>(context.get());
    }
};

} // namespace std
#endif // FOUNDATIONKIT_MEMORY_ALIGNED_HPP
