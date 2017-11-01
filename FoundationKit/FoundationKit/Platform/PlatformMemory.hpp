/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_FOUNDATIONKIT_PLATFORMMEMORY_HPP
#define FOUNDATIONKIT_FOUNDATIONKIT_PLATFORMMEMORY_HPP

#include <mutex>

#include "FoundationKit/GenericPlatformMacros.hpp"
#if (PLATFORM_ANDROID)
#include "FoundationKit/Platform/Android/AndroidPlatformMemory.hpp"
#elif (PLATFORM_APPLE)
#include "FoundationKit/Platform/Apple/ApplePlatformMemory.hpp"
#elif (PLATFORM_WINDOWS)
#include "FoundationKit/Platform/windows/WindowsPlatformMemory.hpp"
#endif

NS_FK_BEGIN

class ProtectedMemoryLocker
{
public:
    ProtectedMemoryLocker() = delete;
    ProtectedMemoryLocker(const ProtectedMemoryLocker&) = delete;
    ProtectedMemoryLocker& operator = (const ProtectedMemoryLocker&) = delete;
    explicit ProtectedMemoryLocker(ProtectedMemoryPool* pool) :_pool(pool)
    {
        _mutex.lock();
        _pool->UnProtect();
    }

    ~ProtectedMemoryLocker()
    {
        _pool->Protect();
        _mutex.unlock();
    }
private:
    std::mutex                _mutex;
    ProtectedMemoryPool*      _pool;
};

NS_FK_END

#endif // END OF FOUNDATIONKIT_FOUNDATIONKIT_PLATFORMMEMORY_HPP
