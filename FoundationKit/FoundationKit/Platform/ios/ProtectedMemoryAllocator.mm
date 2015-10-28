
#include "ProtectedMemoryAllocator.h"
#include <assert.h>


ProtectedMemoryAllocator::ProtectedMemoryAllocator(vm_size_t pool_size)
    : _poolSize(pool_size)
    , _nextAllocOffset(0)
    , _valid(false)
{
    
    kern_return_t result = vm_allocate(mach_task_self(), &_baseAddress, _poolSize, TRUE);
    _valid = (result == KERN_SUCCESS);
    assert(_valid);
}

ProtectedMemoryAllocator::~ProtectedMemoryAllocator()
{
    vm_deallocate(mach_task_self(), _baseAddress, _poolSize);
}

char *ProtectedMemoryAllocator::allocate(vm_size_t size)
{
    if (_valid && _nextAllocOffset + size <= _poolSize)
    {
        char *p = (char*)_baseAddress + _nextAllocOffset;
        _nextAllocOffset += size;
        return p;
    }
    
    return NULL;  // ran out of memory in our allocation block
}

bool  ProtectedMemoryAllocator::protect()
{
    kern_return_t result = vm_protect(mach_task_self(), _baseAddress, _poolSize, FALSE, VM_PROT_READ);
    return (result == KERN_SUCCESS);
}

bool  ProtectedMemoryAllocator::unProtect()
{
    kern_return_t result = vm_protect(mach_task_self(), _baseAddress, _poolSize, FALSE, VM_PROT_READ | VM_PROT_WRITE);
    return (result == KERN_SUCCESS);
}


bool  ProtectedMemoryAllocator::protect(vm_address_t address, vm_size_t size)
{
    kern_return_t result =  vm_protect(mach_task_self(), address, size, FALSE, VM_PROT_READ);
    return (result == KERN_SUCCESS);
}

// Makes the entire allocation pool read/write.
bool  ProtectedMemoryAllocator::unProtect(vm_address_t address, vm_size_t size)
{
    kern_return_t result =  vm_protect(mach_task_self(), address, size, FALSE, VM_PROT_READ | VM_PROT_WRITE);
    return (result == KERN_SUCCESS);
}











