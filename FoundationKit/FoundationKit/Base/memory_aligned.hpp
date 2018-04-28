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
#include "FoundationKit/Base/types.hpp"
#include "FoundationKit/Base/max_integer_of.hpp"


NS_FK_BEGIN

template<typename T>
using memory_aligned_t = typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type;

// Max memory align value
const std::size_t alignment_of_max_align = std::alignment_of<max_align_t>::value;

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
 * @param ptr			Value to align
 * @param alignment		alignment, must be a power of two
 * @return				Aligned value
 */
template <typename T>
inline constexpr T* align(const T* ptr, int32_t alignment)
{
    return (T*)((UPTRINT(ptr) + alignment - 1) & ~(alignment - 1));
    //return (T*)((UPTRINT(ptr) + alignment - 1) & -alignment);
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
 * @param ptr - The pointer to check.
 * @param alignment		alignment, must be a power of two
 * @return true if the pointer is aligned, false otherwise.
 */
 inline bool is_alignment(const /*volatile*/ void* ptr, std::size_t alignment)noexcept
{
    ASSERTED(is_alignment(alignment),"alignment must be is ");
    return (UPTRINT(ptr) & (alignment - 1)) == 0;
}

 inline void* aligned_alloc(std::size_t size, std::size_t alignment = alignment_of_max_align) noexcept
 {
     assert(is_alignment(alignment));
#if PLATFORM_WINDOWS
     return ::_aligned_malloc(size, alignment);
#elif PLATFORM_APPLE || PLATFORM_LINUX
     enum {
         void_size = sizeof(void*)
     };
     if (alignment < void_size) {
         alignment = void_size;
     }
     void* p;
     if (::posix_memalign(&p, alignment, size) != 0) {
         p = 0;
     }
     return p;

#elif PLATFORM_ANDROID
     return ::memalign(alignment, size);
#endif
 }

 /**
 Causes the space pointed to by `ptr` to be
 deallocated, that is, made available for further
 allocation. If `ptr` is a null pointer, no
 action occurs. Otherwise, if the argument does
 not match a pointer earlier returned by the
 `aligned_alloc` function, or if the space has
 been deallocated by a call to `aligned_free`,
 the behavior is undefined.
 */
 inline void aligned_free(void* ptr) noexcept
 {
#if PLATFORM_WINDOWS
     ::_aligned_free(ptr);
#elif PLATFORM_APPLE || PLATFORM_ANDROID || PLATFORM_LINUX
     ::free(ptr);
#endif
 }

//
///** 
// * Aligned memory allocator, for use with STL types like std::vector.
// * For example:
// *     std::vector<CusTomClass, aligned_allocator<CusTomClass>> myvector;
// *
// */
//template <typename T>
//class aligned_allocator : public std::allocator<T> {
//public:
//    /// Size type.
//    typedef size_t size_type;
//    /// Pointer of type T.
//    typedef T *pointer;
//    /// Const pointer of type T.
//    typedef const T *const_pointer;
//
//    /// Constructs a simd_allocator.
//    aligned_allocator() throw() : std::allocator<T>() {}
//    /// @brief Constructs and copies a simd_allocator.
//    ///
//    /// @param a Allocator to copy.
//    aligned_allocator(const aligned_allocator &a) throw() : std::allocator<T>(a) {}
//    /// @brief Constructs and copies a simd_allocator.
//    ///
//    /// @param a Allocator to copy.
//    template <class U>
//    aligned_allocator(const aligned_allocator<U> &a) throw() : std::allocator<T>(a) {}
//    /// @brief Destructs a simd_allocator.
//    ~aligned_allocator() throw() {}
//
//    /// @brief Obtains an allocator of a different type.
//    template <typename _Tp1>
//    struct rebind 
//    {
//        /// @brief Allocator of type _Tp1.
//        typedef aligned_allocator<_Tp1> other;
//    };
//
//    /// @brief Allocate memory for object T.
//    ///
//    /// @param n Number of types to allocate.
//    /// @return Pointer to the newly allocated memory.
//    pointer allocate(size_type n) 
//    {
//        return reinterpret_cast<pointer>(allocate_aligned(n * sizeof(T)));
//    }
//
//    /// Deallocate memory referenced by pointer p.
//    ///
//    /// @param p Pointer to memory to deallocate.
//    void deallocate(pointer p, size_type) { free_aligned(p); }
//};
//
//
//template< typename _Ty >
//class aligned_ptr final 
//{
//public:
//    typedef memory_aligned_t<_Ty> aligned_value_type;
//    typedef _Ty		              value_type;
//    typedef _Ty&		          reference;
//    typedef _Ty* 		          pointer;
//    typedef const _Ty& 	          const_reference;
//    typedef	const _Ty*            const_pointer;
//    typedef const _Ty* const      const_pointer_const;
//
//    template< typename ...Args >
//    explicit aligned_ptr(Args &&...args) 
//    {
//        void* dataPtr = ::operator new(sizeof(aligned_value_type));
//        new (dataPtr)value_type(std::forward< Args >(args)...);
//        context.template reset<aligned_value_type>(reinterpret_cast<aligned_value_type*>(dataPtr), [](aligned_value_type* ptr)
//        {
//            reinterpret_cast<pointer>(ptr)->~value_type();
//            ::operator delete(ptr);
//        });
//    }
//
//    ~aligned_ptr() 
//    {
//
//    }
//
//    aligned_ptr(aligned_ptr &&other) noexcept 
//        :context(std::move(other.context))
//    {
//    }
//   
//    aligned_ptr& operator=(aligned_ptr &&other) noexcept
//    {
//        context = std::move(other.context);
//        return *this;
//    }
//
//    aligned_ptr(const aligned_ptr& other)
//    {
//        context = other.context;
//    }
//
//    aligned_ptr& operator=(const aligned_ptr& other)
//    {
//        context = other.context;
//        return *this;
//    }
//
//    reference operator*() 
//    {
//        return *reinterpret_cast<pointer>(context.get());
//    }
//
//    const_reference operator*() const 
//    {
//        return *reinterpret_cast<const_pointer>(context.get());
//    }
//
//    pointer operator->() 
//    {
//        return reinterpret_cast<pointer>(context.get());
//    }
//
//    const_pointer operator->() const 
//    {
//        return reinterpret_cast<const_pointer>(context.get());
//    }
//
//    pointer get() 
//    {
//        return reinterpret_cast<pointer>(context.get());
//    }
//
//    const_pointer get() const 
//    {
//        return reinterpret_cast<const_pointer>(context.get());
//    }
//private:
//    std::shared_ptr<aligned_value_type> context;
//};

NS_FK_END
#endif // FOUNDATIONKIT_MEMORY_ALIGNED_HPP



