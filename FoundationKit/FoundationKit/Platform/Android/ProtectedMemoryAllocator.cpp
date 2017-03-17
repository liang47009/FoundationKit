#include "ProtectedMemoryAllocator.hpp"
#include <assert.h>
#include <sys/mman.h>
#include <unistd.h>
ProtectedMemoryAllocator::ProtectedMemoryAllocator(size_t pool_size)
    : _poolSize(pool_size)
    , _nextAllocOffset(0)
    , _valid(false)
{
    void* pAddress = mmap(nullptr, _poolSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    _valid = (pAddress != 0 && pAddress != MAP_FAILED);
    _baseAddress = reinterpret_cast<vm_address_t>(pAddress);
    assert(_valid);
}

ProtectedMemoryAllocator::~ProtectedMemoryAllocator()
{
    munmap(reinterpret_cast<void*>(_baseAddress), _poolSize);
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
    return mprotect(reinterpret_cast<void*>(_baseAddress), _poolSize, PROT_READ) == 0;
}

bool ProtectedMemoryAllocator::unProtect()
{
    return mprotect(reinterpret_cast<void*>(_baseAddress), _poolSize, PROT_READ | PROT_WRITE) == 0;
}

bool ProtectedMemoryAllocator::protect(vm_address_t address, vm_size_t size)
{
    return mprotect(reinterpret_cast<void*>(address), size, PROT_READ) == 0;
}

bool ProtectedMemoryAllocator::unProtect(vm_address_t address, vm_size_t size)
{
    return mprotect(reinterpret_cast<void*>(address), size, PROT_READ | PROT_WRITE) == 0;
}





