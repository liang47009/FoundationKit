/****************************************************************************
 Copyright (c) 2015 libo All rights reserved.
 
 losemymind.libo@gmail.com
 
 ****************************************************************************/
#ifndef LOSEMYMIND_PROTECTEDMEMORYALLOCATOR_H
#define LOSEMYMIND_PROTECTEDMEMORYALLOCATOR_H


#pragma once

#include <mach/mach.h>
#include <pthread.h>
#include <assert.h>

class ProtectedMemoryAllocator
{
public:
    ProtectedMemoryAllocator(vm_size_t pool_size);
    ~ProtectedMemoryAllocator();
    
    // Returns a pointer to an allocation of size n within the pool.
    // Fails by returning NULL is no more space is available.
    // Please note that the pointers returned from this method should not
    // be freed in any way (for example by calling free() on them ).
    char *         Allocate(vm_size_t size);
    
    // Returns the base address of the allocation pool.
    char *         GetBaseAddress() { return (char*)_baseAddress; }
    
    // Returns the size of the allocation pool, including allocated
    // plus free space.
    vm_size_t      GetTotalSize() { return _poolSize; }
    
    // Returns the number of bytes already allocated in the pool.
    vm_size_t      GetAllocatedSize() { return _nextAllocOffset; }
    
    // Returns the number of bytes available for allocation.
//    vm_size_t      getFreeSize() { return _poolSize - _nextAllocOffset; }
    
    // Makes the entire allocation pool read-only including, of course,
    // all allocations made from the pool.
    bool          Protect();
    
    // Makes the entire allocation pool read/write.
    bool          UnProtect();
    
    static bool   Protect(vm_address_t address, vm_size_t size);
    
    // Makes the entire allocation pool read/write.
    static bool   UnProtect(vm_address_t address, vm_size_t size);
    
    
private:
    vm_size_t      _poolSize;
    vm_address_t   _baseAddress;
    vm_size_t      _nextAllocOffset;
    bool           _valid;
};

class ProtectedMemoryLocker
{
public:
    ProtectedMemoryLocker() = delete;
    ProtectedMemoryLocker(const ProtectedMemoryLocker&)= delete;
    ProtectedMemoryLocker& operator=(const ProtectedMemoryLocker&)= delete;
    
    ProtectedMemoryLocker(pthread_mutex_t *mutex, ProtectedMemoryAllocator *allocator)
    : _mutex(mutex)
    , _allocator(allocator)
    {
        // Lock the mutex
        __attribute__((unused)) int rv = pthread_mutex_lock(_mutex);
        assert(rv == 0);
        // Unprotect the memory
        _allocator->UnProtect();
    }
    
    ~ProtectedMemoryLocker()
    {
        // First protect the memory
        _allocator->Protect();
        // Then unlock the mutex
        __attribute__((unused)) int rv = pthread_mutex_unlock(_mutex);
        assert(rv == 0);
    };
    
private:
    pthread_mutex_t           *_mutex;
    ProtectedMemoryAllocator  *_allocator;
};
#endif // LOSEMYMIND_PROTECTEDMEMORYALLOCATOR_H


