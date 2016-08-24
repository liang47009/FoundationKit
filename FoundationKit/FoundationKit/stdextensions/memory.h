/****************************************************************************
 Copyright (c) 2016 libo All rights reserved.

 losemymind.libo@gmail.com

****************************************************************************/
#pragma once

#include "FoundationKit/stdextensions/stdconfig.h"

#if ENABLE_STD_EXTENSIONS

#include <cstddef>      // NULL
#include <algorithm>    // std::swap

// can be replaced by other error mechanism
#include <cassert>

#define SHARED_ASSERT(x)    assert(x)

namespace std
{

/**
 * @brief implementation of reference counter for the following minimal smart pointer.
 *
 * shared_ptr_count is a container for the allocated pn reference counter.
 */
class shared_ptr_count
{
public:
    shared_ptr_count() :
        pn(NULL)
    {
    }
    shared_ptr_count(const shared_ptr_count& count) :
        pn(count.pn)
    {
    }
    /// @brief Swap method for the copy-and-swap idiom (copy constructor and swap method)
    void swap(shared_ptr_count& lhs) throw() // never throws
    {
        std::swap(pn, lhs.pn);
    }
    /// @brief getter of the underlying reference counter
    long use_count(void) const throw() // never throws
    {
        long count = 0;
        if (NULL != pn)
        {
            count = *pn;
        }
        return count;
    }
    /// @brief acquire/share the ownership of the pointer, initializing the reference counter
    template<class U>
    void acquire(U* p) // may throw std::bad_alloc
    {
        if (NULL != p)
        {
            if (NULL == pn)
            {
                try
                {
                    pn = new long(1); // may throw std::bad_alloc
                }
                catch (std::bad_alloc&)
                {
                    delete p;
                    throw; // rethrow the std::bad_alloc
                }
            }
            else
            {
                ++(*pn);
            }
        }
    }
    /// @brief release the ownership of the px pointer, destroying the object when appropriate
    template<class U>
    void release(U* p) throw() // never throws
    {
        if (NULL != pn)
        {
            --(*pn);
            if (0 == *pn)
            {
                delete p;
                delete pn;
            }
            pn = NULL;
        }
    }

public:
    long*   pn; //!< Reference counter
};


/**
 * @brief minimal implementation of smart pointer, a subset of the C++11 std::shared_ptr or boost::shared_ptr.
 *
 * shared_ptr is a smart pointer retaining ownership of an object through a provided pointer,
 * and sharing this ownership with a reference counter.
 * It destroys the object when the last shared pointer pointing to it is destroyed or reset.
 */
template<class T>
class shared_ptr
{
public:
    /// The type of the managed object, aliased as member type
    typedef T element_type;
	typedef shared_ptr<T> _Myt;

    /// @brief Default constructor
    shared_ptr(int nullptr_t = NULL) throw() : // never throws
        px(NULL),
        pn()
    {
    }
    /// @brief Constructor with the provided pointer to manage
    explicit shared_ptr(T* p) : // may throw std::bad_alloc
      //px(p), would be unsafe as acquire() may throw, which would call release() in destructor
        pn()
    {
        acquire(p);   // may throw std::bad_alloc
    }
    /// @brief Constructor to share ownership. Warning : to be used for pointer_cast only ! (does not manage two separate <T> and <U> pointers)
    template <class U>
    shared_ptr(const shared_ptr<U>& ptr, T* p) :
     //px(p), would be unsafe as acquire() may throw, which would call release() in destructor
       pn(ptr.pn)
    {
       acquire(p);   // may throw std::bad_alloc
    }
    /// @brief Copy constructor to convert from another pointer type
    template <class U>
    shared_ptr(const shared_ptr<U>& ptr) throw() : // never throws (see comment below)
      //px(ptr.px),
        pn(ptr.pn)
    {
        SHARED_ASSERT((NULL == ptr.px) || (0 != ptr.pn.use_count())); // must be coherent : no allocation allowed in this path
        acquire(static_cast<typename shared_ptr<T>::element_type*>(ptr.px));   // will never throw std::bad_alloc
    }
    /// @brief Copy constructor (used by the copy-and-swap idiom)
    shared_ptr(const _Myt& ptr) throw() : // never throws (see comment below)
       //px(ptr.px),
        pn(ptr.pn)
    {
        SHARED_ASSERT((NULL == ptr.px) || (0 != ptr.pn.use_count())); // must be cohérent : no allocation allowed in this path
        acquire(ptr.px);   // will never throw std::bad_alloc
    }

	/// @brief Assignment operator using the copy-and-swap idiom (copy constructor and swap method)
	_Myt& operator=(const _Myt& ptr) throw() // never throws
	{
		shared_ptr(ptr).swap(*this);
		return *this;
	}

    /// @brief the destructor releases its ownership
    inline ~shared_ptr(void) throw() // never throws
    {
        release();
    }
    /// @brief this reset releases its ownership
    inline void reset(void) throw() // never throws
    {
        release();
    }
    /// @brief this reset release its ownership and re-acquire another one
    void reset(T* p) // may throw std::bad_alloc
    {
        SHARED_ASSERT((NULL == p) || (px != p)); // auto-reset not allowed
        release();
        acquire(p); // may throw std::bad_alloc
    }

    /// @brief Swap method for the copy-and-swap idiom (copy constructor and swap method)
    void swap(shared_ptr& lhs) throw() // never throws
    {
        std::swap(px, lhs.px);
        pn.swap(lhs.pn);
    }

    // reference counter operations :
    inline operator bool() const throw() // never throws
    {
        return (0 < pn.use_count());
    }
    inline bool unique(void)  const throw() // never throws
    {
        return (1 == pn.use_count());
    }
    long use_count(void)  const throw() // never throws
    {
        return pn.use_count();
    }

    // underlying pointer operations :
    inline T& operator*()  const throw() // never throws
    {
        SHARED_ASSERT(NULL != px);
        return *px;
    }
    inline T* operator->() const throw() // never throws
    {
        SHARED_ASSERT(NULL != px);
        return px;
    }
    inline T* get(void)  const throw() // never throws
    {
        // no assert, can return NULL
        return px;
    }

private:
    /// @brief acquire/share the ownership of the px pointer, initializing the reference counter
    inline void acquire(T* p) // may throw std::bad_alloc
    {
        pn.acquire(p); // may throw std::bad_alloc
        px = p; // here it is safe to acquire the ownership of the provided raw pointer, where exception cannot be thrown any more
    }

    /// @brief release the ownership of the px pointer, destroying the object when appropriate
    inline void release(void) throw() // never throws
    {
        pn.release(px);
        px = NULL;
    }

private:
    // This allow pointer_cast functions to share the reference counter between different shared_ptr types
    template<class U>
    friend class shared_ptr;

private:
    T*                  px; //!< Native pointer
    shared_ptr_count    pn; //!< Reference counter
};


// comparaison operators
template<class T, class U> inline bool operator==(const shared_ptr<T>& l, const shared_ptr<U>& r) throw() // never throws
{
    return (l.get() == r.get());
}
template<class T, class U> inline bool operator!=(const shared_ptr<T>& l, const shared_ptr<U>& r) throw() // never throws
{
    return (l.get() != r.get());
}
template<class T, class U> inline bool operator<=(const shared_ptr<T>& l, const shared_ptr<U>& r) throw() // never throws
{
    return (l.get() <= r.get());
}
template<class T, class U> inline bool operator<(const shared_ptr<T>& l, const shared_ptr<U>& r) throw() // never throws
{
    return (l.get() < r.get());
}
template<class T, class U> inline bool operator>=(const shared_ptr<T>& l, const shared_ptr<U>& r) throw() // never throws
{
    return (l.get() >= r.get());
}
template<class T, class U> inline bool operator>(const shared_ptr<T>& l, const shared_ptr<U>& r) throw() // never throws
{
    return (l.get() > r.get());
}



// static cast of shared_ptr
template<class T, class U>
shared_ptr<T> static_pointer_cast(const shared_ptr<U>& ptr) // never throws
{
    return shared_ptr<T>(ptr, static_cast<typename shared_ptr<T>::element_type*>(ptr.get()));
}

// dynamic cast of shared_ptr
template<class T, class U>
shared_ptr<T> dynamic_pointer_cast(const shared_ptr<U>& ptr) // never throws
{
    T* p = dynamic_cast<typename shared_ptr<T>::element_type*>(ptr.get());
    if (NULL != p)
    {
        return shared_ptr<T>(ptr, p);
    }
    else
    {
        return shared_ptr<T>();
    }
}






/**
 * @brief fake implementation to use in place of a C++11 std::move() when compiling on an older compiler.
 *
 * @see http://www.cplusplus.com/reference/utility/move/
*/
template <typename T>
inline T& move(T& v)
{
    return v;
}


/**
 * @brief minimal implementation of smart pointer, a subset of the C++11 std::unique_ptr or boost::unique_ptr.
 *
 * unique_ptr is a smart pointer retaining ownership of an object through a provided pointer,
 * and sharing this ownership with a reference counter.
 * It destroys the object when the last shared pointer pointing to it is destroyed or reset.
 */
template<class T>
class unique_ptr
{
public:
    /// The type of the managed object, aliased as member type
    typedef T element_type;

    /// @brief Default constructor
    unique_ptr(void) throw() : // never throws
        px(NULL)
    {
    }
    /// @brief Constructor with the provided pointer to manage
    explicit unique_ptr(T* p) throw() : // never throws
        px(p)
    {
    }
    /// @brief Copy constructor to convert from another pointer type
    /* TODO MSVC error C2248: 'unique_ptr<B>::px' : unique_ptr<A> cannot access private member declared in class 'unique_ptr<B>'
    template <class U>
    unique_ptr(const unique_ptr<U>& ptr) throw() : // never throws
        px(static_cast<typename unique_ptr<T>::element_type*>(ptr.px))
    {
        const_cast<unique_ptr<U>&>(ptr).px = NULL; // const-cast to force ownership transfer!
    }
    */
    /// @brief Copy constructor (used by the copy-and-swap idiom)
    unique_ptr(const unique_ptr& ptr) throw() : // never throws
        px(ptr.px)
    {
        const_cast<unique_ptr&>(ptr).px = NULL; // const-cast to force ownership transfer!
    }
    /// @brief Assignment operator using the copy-and-swap idiom (copy constructor and swap method)
    unique_ptr& operator=(unique_ptr ptr) throw() // never throws
    {
        swap(ptr);
        return *this;
    }
    /// @brief the destructor releases its ownership and destroy the object
    inline ~unique_ptr(void) throw() // never throws
    {
        destroy();
    }
    /// @brief this reset releases its ownership and destroy the object
    inline void reset(void) throw() // never throws
    {
        destroy();
    }
    /// @brief this reset release its ownership and re-acquire another one
    void reset(T* p) throw() // never throws
    {
        SHARED_ASSERT((NULL == p) || (px != p)); // auto-reset not allowed
        destroy();
        px = p;
    }

    /// @brief Swap method for the copy-and-swap idiom (copy constructor and swap method)
    void swap(unique_ptr& lhs) throw() // never throws
    {
        std::swap(px, lhs.px);
    }

    /// @brief release the ownership of the px pointer without destroying the object!
    inline void release(void) throw() // never throws
    {
        px = NULL;
    }

    // reference counter operations :
    inline operator bool() const throw() // never throws
    {
        return (NULL != px); // TODO nullptr
    }

    // underlying pointer operations :
    inline T& operator*()  const throw() // never throws
    {
        SHARED_ASSERT(NULL != px);
        return *px;
    }
    inline T* operator->() const throw() // never throws
    {
        SHARED_ASSERT(NULL != px);
        return px;
    }
    inline T* get(void)  const throw() // never throws
    {
        // no assert, can return NULL
        return px;
    }

private:
    /// @brief release the ownership of the px pointer and destroy the object
    inline void destroy(void) throw() // never throws
    {
        delete px;
        px = NULL;
    }

    /// @brief hack: const-cast release the ownership of the px pointer without destroying the object!
    inline void release(void) const throw() // never throws
    {
        px = NULL;
    }

private:
    T* px; //!< Native pointer
};


// comparaison operators
template<class T, class U> inline bool operator==(const unique_ptr<T>& l, const unique_ptr<U>& r) throw() // never throws
{
    return (l.get() == r.get());
}
template<class T, class U> inline bool operator!=(const unique_ptr<T>& l, const unique_ptr<U>& r) throw() // never throws
{
    return (l.get() != r.get());
}
template<class T, class U> inline bool operator<=(const unique_ptr<T>& l, const unique_ptr<U>& r) throw() // never throws
{
    return (l.get() <= r.get());
}
template<class T, class U> inline bool operator<(const unique_ptr<T>& l, const unique_ptr<U>& r) throw() // never throws
{
    return (l.get() < r.get());
}
template<class T, class U> inline bool operator>=(const unique_ptr<T>& l, const unique_ptr<U>& r) throw() // never throws
{
    return (l.get() >= r.get());
}
template<class T, class U> inline bool operator>(const unique_ptr<T>& l, const unique_ptr<U>& r) throw() // never throws
{
    return (l.get() > r.get());
}

} //namespace std


#endif // #if ENABLE_STD_EXTENSIONS

