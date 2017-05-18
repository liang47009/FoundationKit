/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_MUTEX_H
#define FOUNDATIONKIT_MUTEX_H

#include "stddefine.h"

// Note: TryEnterCriticalSection only exists on NT-derived systems.
// But we do not run on Win9x currently anyway, so just accept it.
#if !defined _WIN32_WINNT || _WIN32_WINNT < 0x0400
extern "C" WINBASEAPI BOOL WINAPI TryEnterCriticalSection( IN OUT LPCRITICAL_SECTION lpCriticalSection );
#endif

namespace std
{

class mutex 
{
public:
    mutex(): mAlreadyLocked(false)
    {
        InitializeCriticalSection(&mHandle);
    }

    ~mutex()
    {
        DeleteCriticalSection(&mHandle);
    }

    inline void lock()
    {
        EnterCriticalSection(&mHandle);
        while (mAlreadyLocked) Sleep(1000);
        mAlreadyLocked = true;
    }

    inline bool try_lock()
    {
      bool ret = (TryEnterCriticalSection(&mHandle) ? true : false);
      if(ret && mAlreadyLocked)
      {
        LeaveCriticalSection(&mHandle);
        ret = false;
      }
      return ret;
    }

    inline void unlock()
    {
        mAlreadyLocked = false;
        LeaveCriticalSection(&mHandle);
    }
private:
    mutex(const mutex&); 
    mutex& operator=(const mutex&);
    CRITICAL_SECTION mHandle;
    bool mAlreadyLocked;
    friend class condition_variable;
};

class recursive_mutex 
{
public:
    recursive_mutex()
    {
        InitializeCriticalSection(&mHandle);
    }

    ~recursive_mutex()
    {
        DeleteCriticalSection(&mHandle);
    }

    inline void lock()
    {
        EnterCriticalSection(&mHandle);
    }

    inline bool try_lock()
    {
      return TryEnterCriticalSection(&mHandle) ? true : false;
    }

    inline void unlock()
    {
        LeaveCriticalSection(&mHandle);
    }

private:
    recursive_mutex(const recursive_mutex&); 
    recursive_mutex& operator=(const recursive_mutex&);
    CRITICAL_SECTION mHandle;
    friend class condition_variable;
};

// LOCK PROPERTIES
struct adopt_lock_t
{	// indicates adopt lock
};

struct defer_lock_t
{	// indicates defer lock
};

struct try_to_lock_t
{	// indicates try to lock
};

template <class T>
class lock_guard {
public:
    typedef T mutex_type;
    lock_guard() : _MyMutex(0) {}

    explicit lock_guard(mutex_type &aMutex)
    {
        _MyMutex = &aMutex;
        _MyMutex->lock();
    }

    explicit lock_guard(const mutex_type &aMutex)
    {
        _MyMutex = (mutex_type*)&aMutex;
        _MyMutex->lock();
    }

    lock_guard(_Mutex& _Mtx, adopt_lock_t)
        : _MyMutex(_Mtx)
    {	// construct but don't lock
    }
    /// The destructor unlocks the mutex.
    ~lock_guard()
    {
        if(_MyMutex)
            _MyMutex->unlock();
    }

private:
    mutex_type * _MyMutex;
};


template<class _Mutex>
class unique_lock
{	// whizzy class with destructor that unlocks mutex
public:
    typedef unique_lock<_Mutex> _Myt;
    typedef _Mutex mutex_type;

    // CONSTRUCT, ASSIGN, AND DESTROY
    unique_lock() _NOEXCEPT
        : _Pmtx(0), _Owns(false)
    {	// default construct
    }

    explicit unique_lock(_Mutex& _Mtx)
        : _Pmtx(&_Mtx), _Owns(false)
    {	// construct and lock
        _Pmtx->lock();
        _Owns = true;
    }

    unique_lock(_Mutex& _Mtx, adopt_lock_t)
        : _Pmtx(&_Mtx), _Owns(true)
    {	// construct and assume already locked
    }

    unique_lock(_Mutex& _Mtx, defer_lock_t) _NOEXCEPT
        : _Pmtx(&_Mtx), _Owns(false)
    {	// construct but don't lock
    }

    unique_lock(_Mutex& _Mtx, try_to_lock_t)
        : _Pmtx(&_Mtx), _Owns(_Pmtx->try_lock())
    {	// construct and try to lock
    }

    ~unique_lock() _NOEXCEPT
    {	// clean up
        if (_Owns)
            _Pmtx->unlock();
    }

    // LOCK AND UNLOCK
    void lock()
    {	// lock the mutex
        _Pmtx->lock();
        _Owns = true;
    }

    bool try_lock() _NOEXCEPT
    {	// try to lock the mutex
        _Owns = _Pmtx->try_lock();
        return (_Owns);
    }

    void unlock()
    {	// unlock the mutex
        _Pmtx->unlock();
        _Owns = false;
    }

    // MUTATE
    void swap(unique_lock& _Other) _NOEXCEPT
    {	// swap with _Other
        _STD swap(_Pmtx, _Other._Pmtx);
        _STD swap(_Owns, _Other._Owns);
    }

    _Mutex *release() _NOEXCEPT
    {	// disconnect
        _Mutex *_Res = _Pmtx;
        _Pmtx = 0;
        _Owns = false;
        return (_Res);
    }

    // OBSERVE
    bool owns_lock() const _NOEXCEPT
    {	// return true if this object owns the lock
        return (_Owns);
    }

    operator bool() const _NOEXCEPT
    {	// return true if this object owns the lock
        return (_Owns);
    }

    _Mutex *mutex() const _NOEXCEPT
    {	// return pointer to managed mutex
        return (_Pmtx);
    }

private:
    unique_lock(const unique_lock&);
    unique_lock& operator=(const unique_lock&);
    _Mutex *_Pmtx;
    bool _Owns;
};

// SWAP
template<class _Mutex>
void swap(unique_lock<_Mutex>& _Left,
          unique_lock<_Mutex>& _Right) _NOEXCEPT
{	// swap _Left and _Right
    _Left.swap(_Right);
}

} //namespace std
#endif // FOUNDATIONKIT_MUTEX_H
