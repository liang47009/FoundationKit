/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_PROTECTEDMEMORYALLOCATOR_HPP
#define FOUNDATIONKIT_PROTECTEDMEMORYALLOCATOR_HPP

#pragma once
#include "FoundationKit/GenericPlatformMacros.hpp"

typedef uintptr_t     vm_offset_t;
typedef uintptr_t     vm_size_t;
typedef vm_offset_t   vm_address_t;

class ProtectedMemoryAllocator
{
    ProtectedMemoryAllocator();
    ProtectedMemoryAllocator(const ProtectedMemoryAllocator&);
    ProtectedMemoryAllocator& operator = (const ProtectedMemoryAllocator&);
public:
	explicit ProtectedMemoryAllocator(size_t pool_size);
	~ProtectedMemoryAllocator();

	char*       allocate(vm_size_t size);

	char*       getBaseAddress(){ return reinterpret_cast<char*>(_baseAddress); }

	vm_size_t   getTotalSize(){ return _poolSize; }

	vm_size_t   getAllocatedSize(){ return _nextAllocOffset; }

	bool        protect();

	bool        unProtect();

	static bool protect(vm_address_t address, vm_size_t size);

	static bool unProtect(vm_address_t address, vm_size_t size);

private:
	vm_size_t         _poolSize;
	vm_address_t      _baseAddress;
	vm_size_t         _nextAllocOffset;
	bool              _valid;
};


class ProtectedMemoryLocker
{
    ProtectedMemoryLocker();
    ProtectedMemoryLocker(const ProtectedMemoryLocker&);
    ProtectedMemoryLocker& operator = (const ProtectedMemoryLocker&);
public:

	explicit ProtectedMemoryLocker(ProtectedMemoryAllocator* allocator) :_allocator(allocator)
	{
		_mutex.lock();
		_allocator->unProtect();
	}

	~ProtectedMemoryLocker()
	{
		_allocator->protect();
		_mutex.unlock();
	}

private:
	std::mutex                _mutex;
	ProtectedMemoryAllocator *_allocator;
};

#endif // FOUNDATIONKIT_PROTECTEDMEMORYALLOCATOR_HPP
