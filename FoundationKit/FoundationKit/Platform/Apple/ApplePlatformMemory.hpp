/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_FOUNDATIONKIT_APPLEPLATFORMMEMORY_HPP
#define FOUNDATIONKIT_FOUNDATIONKIT_APPLEPLATFORMMEMORY_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"
#if (PLATFORM_IOS || PLATFORM_MAC)

#include <assert.h>
#include <memory>
#include <malloc/malloc.h>

NS_FK_BEGIN


inline size_t MallocUsableSize(void* ptr)
{
    return malloc_size(ptr);
}


inline bool MemoryProtect(vm_address_t address, vm_size_t size)
{
    kern_return_t result = vm_protect(mach_task_self(), address, size, FALSE, VM_PROT_READ | VM_PROT_EXECUTE | VM_PROT_COPY);
    return (result == KERN_SUCCESS);
}

inline bool MemoryUnprotect(vm_address_t address, vm_size_t size)
{
    kern_return_t result = vm_protect(mach_task_self(), address, size, FALSE, VM_PROT_READ | VM_PROT_WRITE | VM_PROT_COPY);
    return (result == KERN_SUCCESS);
}

class ProtectedMemoryPool
{
public:
    ProtectedMemoryPool() = delete;
    ProtectedMemoryPool(const ProtectedMemoryPool&) = delete;
    ProtectedMemoryPool& operator = (const ProtectedMemoryPool&) = delete;

    explicit ProtectedMemoryPool(vm_size_t pool_size)
    {
        kern_return_t result = vm_allocate(mach_task_self(), &_baseAddress, _poolSize, TRUE);
        _valid = (result == KERN_SUCCESS);
        assert(_valid);
    }
    ~ProtectedMemoryPool()
    {
        vm_deallocate(mach_task_self(), _baseAddress, _poolSize);
    }

    char* Allocate(vm_size_t size)
    {
        if (_valid && _nextAllocOffset + size <= _poolSize)
        {
            char *p = (char*)_baseAddress + _nextAllocOffset;
            _nextAllocOffset += size;
            return p;
        }
        return NULL;  // ran out of memory in our allocation block
    }

    char* GetBaseAddress() 
    { 
        return reinterpret_cast<char*>(_baseAddress);
    }

    vm_size_t GetTotalSize()
    { 
        return _poolSize; 
    }

    vm_size_t GetAllocatedSize() 
    { 
        return _nextAllocOffset; 
    }

    bool Protect()
    {
        return MemoryProtect(_baseAddress, _poolSize);
    }

    bool UnProtect()
    {
        return MemoryUnprotect(_baseAddress, _poolSize);
    }

    template<class _Objty, class... _Types>
    _Objty* Construct( _Types&&... _Args)
    {	
        void* _Ptr = Allocate(sizeof(_Objty));
        ::new (_Ptr) _Objty(std::forward<_Types>(_Args)...);
        return _Ptr;
    }
private:
    vm_size_t         _poolSize;
    vm_address_t      _baseAddress;
    vm_size_t         _nextAllocOffset;
    bool              _valid;
};


NS_FK_END

#endif // #if (PLATFORM_IOS || PLATFORM_MAC)

#endif // END OF FOUNDATIONKIT_FOUNDATIONKIT_APPLEPLATFORMMEMORY_HPP