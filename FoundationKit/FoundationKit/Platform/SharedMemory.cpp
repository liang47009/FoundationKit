#include "SharedMemory.h"
#include "SharedMemoryImpl.h"
NS_FK_BEGIN

SharedMemory::SharedMemory()
    : _impl(new SharedMemoryImpl())
{

}

SharedMemory::~SharedMemory()
{

}

SharedMemory::SharedMemory(const std::string& name, std::size_t size, AccessMode mode, const void* addrHint /*= 0*/, bool server /*= true*/)
    : _impl(new SharedMemoryImpl(name, size, mode, addrHint, server))
{

}

SharedMemory::SharedMemory(const SharedMemory& other)
{
    _impl = other._impl;
}

SharedMemory::SharedMemory(SharedMemory&& other)
{
    _impl = std::move(other._impl);
}

SharedMemory& SharedMemory::operator=(const SharedMemory& other)
{
    _impl = other._impl;
    return (*this);
}

SharedMemory& SharedMemory::operator=(SharedMemory&& other)
{
    _impl = std::move(other._impl);
    return (*this);
}

void SharedMemory::swap(SharedMemory& other)
{
    _impl.swap(other._impl);
}

char* SharedMemory::begin() const
{
    return _impl->begin();
}

char* SharedMemory::end() const
{
    return _impl->end();
}

bool SharedMemory::lock()
{
    return _impl->lock();
}

bool SharedMemory::unlock()
{
    return _impl->unlock();
}

NS_FK_END

