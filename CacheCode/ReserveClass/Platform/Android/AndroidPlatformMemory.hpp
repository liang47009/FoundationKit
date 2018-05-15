/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_FOUNDATIONKIT_ANDROIDPLATFORMMEMORY_HPP
#define FOUNDATIONKIT_FOUNDATIONKIT_ANDROIDPLATFORMMEMORY_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_ANDROID

#include <assert.h>
#include <memory>
#include <sys/mman.h>
#include <unistd.h>

NS_FK_BEGIN

typedef uintptr_t     vm_offset_t;
typedef uintptr_t     vm_size_t;
typedef vm_offset_t   vm_address_t;

struct chunk {
    size_t psize, csize;
    struct chunk *next, *prev;
};

#if __ANDROID_API__ < 17
#define OVERHEAD (2*sizeof(size_t))
#define CHUNK_SIZE(c) ((c)->csize & -2)
#define MEM_TO_CHUNK(p) (struct chunk *)((char *)(p) - OVERHEAD)
#endif

inline size_t MallocUsableSize(void* ptr)
{
#if __ANDROID_API__ < 17
    return ptr ? CHUNK_SIZE(MEM_TO_CHUNK(ptr)) - OVERHEAD : 0;
#else
    return malloc_usable_size(ptr);
#endif
}


inline bool MemoryProtect(vm_address_t address, vm_size_t size)
{
    return mprotect(reinterpret_cast<void*>(address), size, PROT_READ) == 0;
}

inline bool MemoryUnprotect(vm_address_t address, vm_size_t size)
{
    return mprotect(reinterpret_cast<void*>(address), size, PROT_READ | PROT_WRITE) == 0;
}

class ProtectedMemoryPool
{
public:
    ProtectedMemoryPool() = delete;
    ProtectedMemoryPool(const ProtectedMemoryPool&) = delete;
    ProtectedMemoryPool& operator = (const ProtectedMemoryPool&) = delete;

    explicit ProtectedMemoryPool(vm_size_t pool_size)
    {
        void* pAddress = mmap(nullptr, _poolSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        _valid = (pAddress != 0 && pAddress != MAP_FAILED);
        _baseAddress = reinterpret_cast<vm_address_t>(pAddress);
        assert(_valid);
    }
    ~ProtectedMemoryPool()
    {
        munmap(reinterpret_cast<void*>(_baseAddress), _poolSize);
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

#endif // #if PLATFORM_ANDROID

#endif // END OF FOUNDATIONKIT_FOUNDATIONKIT_ANDROIDPLATFORMMEMORY_HPP
