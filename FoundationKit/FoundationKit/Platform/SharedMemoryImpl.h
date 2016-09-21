#ifndef LOSEMYMIND_SHAREDMEMORYIMPL_H
#define LOSEMYMIND_SHAREDMEMORYIMPL_H


#include <string>
#include "FoundationKit/Base/noncopyable.hpp"
#include "FoundationKit/Base/nonmovable.hpp"
#include"SharedMemory.h"

NS_FK_BEGIN

class SharedMemoryImpl:noncopyable,nonmovable
{
public:


    /// Default constructor creates an unmapped SharedMemory object. 
    /// No clients can connect to an unmapped SharedMemory object.
    SharedMemoryImpl();

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
    SharedMemoryImpl(const std::string& name, std::size_t size, SharedMemory::AccessMode mode, const void* addrHint = 0, bool server = true);

    /// Returns the start address of the shared memory segment.
    /// Will be NULL for illegal segments.
    char* begin() const;

    /// Returns the one-past-end end address of the shared memory segment. 
    /// Will be NULL for illegal segments.
    char* end() const;

    bool lock();

    bool unlock();

    /// Destroys the SharedMemoryImpl.
    ~SharedMemoryImpl();
protected:
    /// Maps the shared memory object.
    void map(const void* addrHint);

    /// Unmaps the shared memory object.
    void unmap();

    /// Releases the handle for the shared memory segment.
    void close();

private:
    std::size_t _size;
    int         _fd;
    char*       _address;
    SharedMemory::AccessMode _access;
    std::string _name;
    bool        _fileMapped;
    bool        _server;

};

NS_FK_END

#endif // LOSEMYMIND_SHAREDMEMORYIMPL_H
