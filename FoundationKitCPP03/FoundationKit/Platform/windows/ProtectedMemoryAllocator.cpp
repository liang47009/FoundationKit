/****************************************************************************
  Copyright (c) 2017 libo.

  losemymind.libo@gmail.com

****************************************************************************/
#include <windows.h>
#include <assert.h>
#include "FoundationKit/Platform/windows/ProtectedMemoryAllocator.hpp"

ProtectedMemoryAllocator::ProtectedMemoryAllocator(size_t pool_size)
    : _poolSize(pool_size)
    , _nextAllocOffset(0)
    , _valid(false)
{
    void* pAddress = VirtualAlloc(nullptr, _poolSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    _valid = (pAddress != nullptr);
    _baseAddress = (vm_address_t)pAddress;
    assert(_valid);
}

ProtectedMemoryAllocator::~ProtectedMemoryAllocator()
{
    VirtualFree(reinterpret_cast<void*>(_baseAddress), 0, MEM_RELEASE);
}

char* ProtectedMemoryAllocator::allocate(vm_size_t size)
{
    if (_valid && _nextAllocOffset + size <= _poolSize)
    {
        char* p = reinterpret_cast<char*>(_baseAddress + _nextAllocOffset);
        _nextAllocOffset += size;
        return p;
    }
    return nullptr;
}

bool ProtectedMemoryAllocator::protect()
{
    DWORD oldProp;
    BOOL ret = VirtualProtect(reinterpret_cast<void*>(_baseAddress), _poolSize, PAGE_READONLY, &oldProp);
    return ret != 0;
}

bool ProtectedMemoryAllocator::unProtect()
{
    DWORD oldProp;
    BOOL ret = VirtualProtect(reinterpret_cast<void*>(_baseAddress), _poolSize, PAGE_READWRITE, &oldProp);
    return ret != 0;
}

bool ProtectedMemoryAllocator::protect(vm_address_t address, vm_size_t size)
{
    DWORD oldProp;
    BOOL ret = VirtualProtect(reinterpret_cast<void*>(address), size, PAGE_READONLY, &oldProp);
    return ret != 0;
}

bool ProtectedMemoryAllocator::unProtect(vm_address_t address, vm_size_t size)
{
    DWORD oldProp;
    BOOL ret = VirtualProtect(reinterpret_cast<void*>(address), size, PAGE_READWRITE, &oldProp);
    return ret != 0;
}