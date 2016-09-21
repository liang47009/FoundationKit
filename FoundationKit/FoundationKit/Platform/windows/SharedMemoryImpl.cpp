
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
  
}

void SharedMemoryImpl::unmap()
{

}

void SharedMemoryImpl::close()
{

}

bool SharedMemoryImpl::lock()
{
    return true;
}

bool SharedMemoryImpl::unlock()
{
    return true;
}

NS_FK_END


