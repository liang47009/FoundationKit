#ifndef LOSEMYMIND_SHAREDMEMORY_H
#define LOSEMYMIND_SHAREDMEMORY_H

#include <string>
#include <thread>
#include <memory>
#include <mutex>
#include "FoundationKit/GenericPlatformMacros.h"

NS_FK_BEGIN


class SharedMemoryImpl;
class SharedMemory
{

    typedef std::shared_ptr<SharedMemoryImpl> SharedMemoryImplPointer;

public:
    enum AccessMode
    {
        AM_READ = 0,
        AM_WRITE
    };

    /// Default constructor creates an unmapped SharedMemory object. 
    /// No clients can connect to an unmapped SharedMemory object.
    SharedMemory();

    /// Creates or connects to a shared memory object with the given name.
    ///
    /// For maximum portability, name should be a valid Unix filename and not
    /// contain any slashes or backslashes.
    ///
    /// An address hint can be passed to the system, specifying the desired
    /// start address of the shared memory area. Whether the hint
    /// is actually honored is, however, up to the system. Windows platform
    /// will generally ignore the hint.
    ///
    /// If server is set to true, the shared memory region will be unlinked
    /// by calling shm_unlink() (on POSIX platforms) when the SharedMemory object is destroyed.
    /// The server parameter is ignored on Windows platforms.
    SharedMemory(const std::string& name, std::size_t size, AccessMode mode, const void* addrHint = 0, bool server = true);

    /// Creates a SharedMemory object by copying another one.
    SharedMemory(const SharedMemory& other);

    SharedMemory(SharedMemory&& other);

    /// Destroys the SharedMemory.
    ~SharedMemory();

    /// Assigns another SharedMemory object.
    SharedMemory& operator = (const SharedMemory& other);

    SharedMemory& operator = (SharedMemory&& other);

    /// Swaps the SharedMemory object with another one.
    void swap(SharedMemory& other);

    /// Returns the start address of the shared memory segment.
    /// Will be NULL for illegal segments.
    char* begin() const;

    /// Returns the one-past-end end address of the shared memory segment. 
    /// Will be NULL for illegal segments.
    char* end() const;

    bool lock();

    bool unlock();

private:
    SharedMemoryImplPointer _impl;
};

class SharedMemoryLocker
{
public:
    SharedMemoryLocker() = delete;
    SharedMemoryLocker(const SharedMemoryLocker&) = delete;
    SharedMemoryLocker& operator = (const SharedMemoryLocker&) = delete;

    explicit SharedMemoryLocker(SharedMemory* sharedMemoryObj)
        : _shared_memory(sharedMemoryObj)
        , _isLocked(false)
    {
        lock();
    }

    ~SharedMemoryLocker()
    {
        unlock();
    }

    void lock()
    {
        if (!_isLocked)
        {
            _isLocked = true;
            _shared_memory->lock();
        }
    }

    void unlock()
    {
        if (_isLocked)
        {
            _shared_memory->unlock();
            _isLocked = false;
        }
    }

private:
    SharedMemory*             _shared_memory;
    bool                      _isLocked;
};


NS_FK_END

#endif // LOSEMYMIND_SHAREDMEMORY_H




