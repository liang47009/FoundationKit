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
#include "FoundationKit/Base/max_integer_of.hpp"


NS_FK_BEGIN

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

inline void *allocate_aligned(size_t n) 
{
    // We need to allocate extra bytes to guarantee alignment,
    // and to store the pointer to the original buffer.
    uint8_t *buf = reinterpret_cast<uint8_t *>(malloc(n + alignment_of_max_align));
    if (!buf) return NULL;
    // Align to next higher multiple of MATHFU_ALIGNMENT.
    uint8_t *aligned_buf = reinterpret_cast<uint8_t *>(
        (reinterpret_cast<size_t>(buf)+alignment_of_max_align) &
        ~(alignment_of_max_align - 1));
    // Write out original buffer pointer before aligned buffer.
    // The assert will fail if the allocator granularity is less than the pointer
    // size, or if alignment_of_max_align doesn't fit two pointers.
    assert(static_cast<size_t>(aligned_buf - buf) > sizeof(void *));
    *(reinterpret_cast<uint8_t **>(aligned_buf)-1) = buf;
    return aligned_buf;
}

/// Deallocate a block of memory allocated with allocate_aligned().
/// @param p Pointer to memory to deallocate.
inline void free_aligned(void *p) 
{
    if (p == NULL) return;
    free(*(reinterpret_cast<uint8_t **>(p)-1));
}


/** 
 * Aligned memory allocator, for use with STL types like std::vector.
 * For example:
 *     std::vector<CusTomClass, aligned_allocator<CusTomClass>> myvector;
 *
 * @param T type allocated by this object.
 */
template <typename T>
class aligned_allocator : public std::allocator<T> {
public:
    /// Size type.
    typedef size_t size_type;
    /// Pointer of type T.
    typedef T *pointer;
    /// Const pointer of type T.
    typedef const T *const_pointer;

    /// Constructs a simd_allocator.
    aligned_allocator() throw() : std::allocator<T>() {}
    /// @brief Constructs and copies a simd_allocator.
    ///
    /// @param a Allocator to copy.
    aligned_allocator(const aligned_allocator &a) throw() : std::allocator<T>(a) {}
    /// @brief Constructs and copies a simd_allocator.
    ///
    /// @param a Allocator to copy.
    /// @param U type of the object allocated by the allocator to copy.
    template <class U>
    aligned_allocator(const aligned_allocator<U> &a) throw() : std::allocator<T>(a) {}
    /// @brief Destructs a simd_allocator.
    ~aligned_allocator() throw() {}

    /// @brief Obtains an allocator of a different type.
    ///
    /// @param  _Tp1 type of the new allocator.
    template <typename _Tp1>
    struct rebind 
    {
        /// @brief Allocator of type _Tp1.
        typedef aligned_allocator<_Tp1> other;
    };

    /// @brief Allocate memory for object T.
    ///
    /// @param n Number of types to allocate.
    /// @return Pointer to the newly allocated memory.
    pointer allocate(size_type n) 
    {
        return reinterpret_cast<pointer>(allocate_aligned(n * sizeof(T)));
    }

    /// Deallocate memory referenced by pointer p.
    ///
    /// @param p Pointer to memory to deallocate.
    void deallocate(pointer p, size_type) { free_aligned(p); }
};


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

NS_FK_END
#endif // FOUNDATIONKIT_MEMORY_ALIGNED_HPP



