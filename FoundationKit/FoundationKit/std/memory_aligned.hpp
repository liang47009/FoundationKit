/****************************************************************************
Copyright (c) 2016 libo All rights reserved.

losemymind.libo@gmail.com

****************************************************************************/
#ifndef LOSEMYMIND_MEMORY_ALIGNED_H
#define LOSEMYMIND_MEMORY_ALIGNED_H
#pragma once
#include "FoundationKit/GenericPlatformMacros.h"
#include "FoundationKit/Base/Types.h"
#include <utility>
#include <type_traits>

USING_NS_FK;

namespace std
{

template<typename T>
using memory_aligned_t = typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type;

// Max memory align value
const std::size_t alignment_of_max_align = std::alignment_of<std::max_align_t>::value;


/**
 * Aligns a value to the nearest higher multiple of 'Alignment', which must be a power of two.
 *
 * @param Ptr			Value to align
 * @param Alignment		Alignment, must be a power of two
 * @return				Aligned value
 */
template <typename T>
inline constexpr T* align(const T* Ptr, int32_t Alignment)
{
    return (T*)(((PTRINT)Ptr + Alignment - 1) & ~(Alignment - 1));
}
/**
 * Checks if a pointer is aligned to the specified alignment.
 * @param Ptr - The pointer to check.
 * @return true if the pointer is aligned, false otherwise.
 */
static FORCEINLINE bool isAligned(const volatile void* Ptr, const uint32 Alignment)
{
    return !(UPTRINT(Ptr) & (Alignment - 1));
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

#endif // LOSEMYMIND_MEMORY_ALIGNED_H
