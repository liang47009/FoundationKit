/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_FOUNDATIONKIT_WINDOWSPLATFORMMEMORY_HPP
#define FOUNDATIONKIT_FOUNDATIONKIT_WINDOWSPLATFORMMEMORY_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"

#if PLATFORM_WINDOWS

#include <assert.h>
#include <memory>

NS_FK_BEGIN

typedef uintptr_t     vm_offset_t;
typedef uintptr_t     vm_size_t;
typedef vm_offset_t   vm_address_t;

inline size_t MallocUsableSize(void* ptr)
{
    return _msize(ptr);
}


inline bool MemoryProtect(vm_address_t address, vm_size_t size)
{
    DWORD oldProp;
    BOOL ret = VirtualProtect(reinterpret_cast<void*>(address), size, PAGE_READONLY, &oldProp);
    return ret != 0;
}

inline bool MemoryUnprotect(vm_address_t address, vm_size_t size)
{
    DWORD oldProp;
    BOOL ret = VirtualProtect(reinterpret_cast<void*>(address), size, PAGE_EXECUTE_READWRITE, &oldProp);
    return ret != 0;
}

class ProtectedMemoryPool
{
public:
    ProtectedMemoryPool() = delete;
    ProtectedMemoryPool(const ProtectedMemoryPool&) = delete;
    ProtectedMemoryPool& operator = (const ProtectedMemoryPool&) = delete;

    explicit ProtectedMemoryPool(vm_size_t pool_size)
    {
        _poolSize = pool_size;
        void* pAddress = VirtualAlloc(nullptr, _poolSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        _valid = (pAddress != nullptr);
        _baseAddress = (vm_address_t)pAddress;
        assert(_valid);
    }
    ~ProtectedMemoryPool()
    {
        VirtualFree(reinterpret_cast<void*>(_baseAddress), 0, MEM_RELEASE);
    }

    char* Allocate(vm_size_t size)
    {
        if (_valid && _nextAllocOffset + size <= _poolSize)
        {
            char* p = reinterpret_cast<char*>(_baseAddress + _nextAllocOffset);
            _nextAllocOffset += size;
            return p;
        }
        return nullptr;
    }

    char*       GetBaseAddress() 
    { 
        return reinterpret_cast<char*>(_baseAddress);
    }

    vm_size_t   GetTotalSize()
    { 
        return _poolSize; 
    }

    vm_size_t   GetAllocatedSize() 
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


#endif // #if PLATFORM_WINDOWS

#endif // END OF FOUNDATIONKIT_FOUNDATIONKIT_WINDOWSPLATFORMMEMORY_HPP
