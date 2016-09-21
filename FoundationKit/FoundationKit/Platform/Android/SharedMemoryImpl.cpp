
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#include "FoundationKit/Base/error_code.hpp"
#include "FoundationKit/Platform/SharedMemoryImpl.h"

NS_FK_BEGIN


SharedMemoryImpl::SharedMemoryImpl()
    : _size(0)
    , _fd(-1)
    , _address(nullptr)
    , _name("/")
    , _fileMapped(false)
    , _server(false)
{

}

SharedMemoryImpl::SharedMemoryImpl(const std::string& name, std::size_t size, SharedMemory::AccessMode mode, const void* addrHint /*= 0*/, bool server /*= true*/)
    : _size(size)
    , _fd(-1)
    , _address(nullptr)
    , _access(mode)
    , _name("/")
    , _fileMapped(false)
    , _server(server)
{
    _name.append(name);

    int flags = _server ? O_CREAT : 0;
    if (_access == SharedMemory::AM_WRITE)
        flags |= O_RDWR;
    else
        flags |= O_RDONLY;

    // open the shared memory segment
    //_fd = ::shm_open(_name.c_str(), flags, S_IRUSR | S_IWUSR);
    _fd = open(_name.c_str(), flags | O_CREAT, S_IRWXU);
    lseek(_fd, _size, SEEK_SET);
    if (_fd == -1)
    {
        std::string errmsg = "Cannot create shared memory object";
        errmsg += _name;
        throw std::runtime_error(errmsg);
    }
        

    // now set the correct size for the segment
    if (_server && -1 == ::ftruncate(_fd, size))
    {
        ::close(_fd);
        _fd = -1;
        //::shm_unlink(_name.c_str());
        std::string errmsg = "Cannot create shared memory object";
        errmsg += _name;
        throw std::runtime_error(errmsg);
    }
    map(addrHint);
}

SharedMemoryImpl::~SharedMemoryImpl()
{
    unmap();
    close();
}

char* SharedMemoryImpl::begin() const
{
    return _address;
}

char* SharedMemoryImpl::end() const
{
    return _address + _size;
}

void SharedMemoryImpl::map(const void* addrHint)
{
    int access = PROT_READ;
    if (_access == SharedMemory::AM_WRITE)
        access |= PROT_WRITE;

    void* addr = ::mmap(const_cast<void*>(addrHint), _size, access, MAP_SHARED, _fd, 0);
    if (addr == MAP_FAILED)
    {
        std::string errmsg = "Cannot map file into shared memory";
        errmsg += _name;
        throw std::runtime_error(errmsg);
    }
    _address = static_cast<char*>(addr);
}

void SharedMemoryImpl::unmap()
{
    if (_address)
    {
        ::munmap(_address, _size);
    }
}

void SharedMemoryImpl::close()
{
    if (_fd != -1)
    {
        ::close(_fd);
        _fd = -1;
    }
    if (!_fileMapped && _server)
    {
        //::shm_unlink(_name.c_str());
    }
}

bool SharedMemoryImpl::lock()
{
    if (_address)
    {
        ::mlock(_address, _size);
    }
    return true;
}

bool SharedMemoryImpl::unlock()
{
    if (_address)
    {
        ::munlock(_address, _size);
    }
    return true;
}

NS_FK_END


