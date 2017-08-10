/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_PROTECTEDMEMORYALLOCATOR_HPP
#define FOUNDATIONKIT_PROTECTEDMEMORYALLOCATOR_HPP

#pragma once
#include "FoundationKit/std/stdheader.h"

typedef uintptr_t     vm_offset_t;
typedef uintptr_t     vm_size_t;
typedef vm_offset_t   vm_address_t;

class ProtectedMemoryAllocator
{
public:
	explicit ProtectedMemoryAllocator(vm_size_t pool_size);
	~ProtectedMemoryAllocator();

	char*       Allocate(vm_size_t size);

	char*       GetBaseAddress(){ return reinterpret_cast<char*>(_baseAddress); }

	vm_size_t   GetTotalSize(){ return _poolSize; }

	vm_size_t   GetAllocatedSize(){ return _nextAllocOffset; }

	bool        Protect();

	bool        UnProtect();

	static bool Protect(vm_address_t address, vm_size_t size);

	static bool UnProtect(vm_address_t address, vm_size_t size);

private:
    ProtectedMemoryAllocator();
    ProtectedMemoryAllocator(const ProtectedMemoryAllocator&);
    ProtectedMemoryAllocator& operator = (const ProtectedMemoryAllocator&);
	vm_size_t         _poolSize;
	vm_address_t      _baseAddress;
	vm_size_t         _nextAllocOffset;
	bool              _valid;
};


class ProtectedMemoryLocker
{
public:
	explicit ProtectedMemoryLocker(ProtectedMemoryAllocator* allocator) :_allocator(allocator)
	{
		_mutex.lock();
		_allocator->UnProtect();
	}

	~ProtectedMemoryLocker()
	{
		_allocator->Protect();
		_mutex.unlock();
	}

private:
    ProtectedMemoryLocker();
    ProtectedMemoryLocker(const ProtectedMemoryLocker&);
    ProtectedMemoryLocker& operator = (const ProtectedMemoryLocker&);
	std::mutex                _mutex;
	ProtectedMemoryAllocator *_allocator;
};

#endif // FOUNDATIONKIT_PROTECTEDMEMORYALLOCATOR_HPP
