/****************************************************************************
Copyright (c) 2013-2014 libo.

losemymind.libo@gmail.com

****************************************************************************/
#ifndef LOSEMYMIND_BUFFER_H
#define LOSEMYMIND_BUFFER_H
#pragma once
#include <stdexcept>
#include <initializer_list>
#include "FoundationKit/GenericPlatformMacros.h"
#include "FoundationKit/Base/Types.h"
NS_FK_BEGIN
/**
* A buffer class that allocates a buffer of a given type and size
* in the constructor and deallocates the buffer in the destructor.
*
* This class is useful everywhere where a temporary buffer
* is needed.
*/
template <class _Elem = unsigned char>
class Buffer
{
public:
    typedef Buffer<_Elem> _Myt;
    typedef uint32        size_type;

    Buffer()
        : _ptr(nullptr)
        , _capacity(0)
        , _readIndex(0)
        , _writeIndex(0)
    {

    }

    Buffer(size_type _Capacity) : Buffer()
    {
        _ptr      = new _Elem[_Capacity];
        _capacity = _Capacity;
    }

    Buffer(_Myt&& _Right) : Buffer()
    {
        move(std::forward<_Myt>(_Right));
    }

    Buffer(const _Myt& _Right) : Buffer()
    {
        assign(_Right);
    }

    Buffer(const _Elem *_Ptr, size_type _Count) : Buffer()
    {
        assign(_Ptr, _Count);
    }

    Buffer(size_type _Count, _Elem _El) : Buffer()
    {
        assign(_Count, _El);
    }

    const _Elem *data() const
    {
        return _ptr;
    }

    size_type capacity() const
    {
        return _capacity;
    }

    size_type capacityBytes() const
    {
        return _capacity*sizeof(_Elem);
    }

    size_type size() const
    {
        return _writeIndex;
    }

    size_type sizeBytes()const
    {
        return _writeIndex * sizeof(_Elem);
    }

    size_type readSize() const 
    {
        return _readIndex;
    }

    size_type readSizeBytes() const
    {
        return _readIndex * sizeof(_Elem);
    }

    size_type writeSize() const
    {
        return _writeIndex;
    }

    size_type writeSizeBytes() const
    {
        return _writeIndex * sizeof(_Elem);
    }

    bool readable() const
    {
        return ((_writeIndex - _readIndex) > 0);
    }

    bool writeable()const
    {
        return ((_capacity - _writeIndex) > 0);
    }

    void setReadSize(size_type _ReadSize)
    {
        LOG_ASSERT(_ReadSize < _writeIndex, "_ReadSize out of range.");
        _readIndex = _ReadSize;
    }

    void write(_Elem _elem)
    {
        LOG_ASSERT(writeable(), "Write out of range.");
        _ptr[_writeIndex++] = _elem;
    }

    _Elem read()
    {
        LOG_ASSERT(readable(), "No data can be read.");
        return _ptr[_readIndex++];
    }

   /**
    * Resizes the buffer capacity and size. If _PreserveContent is true,
    * the content of the old buffer is copied over to the
    * new buffer. The new capacity can be larger or smaller than
    * the current one; if it is smaller, capacity will remain intact.
    * Size will always be set to the new capacity.
    *
    * Buffers only wrapping externally owned storage can not be
    * resized. If resize is attempted on those, IllegalAccessException
    * is thrown.
    */
    void resize(size_type _Newsize, bool _PreserveContent = true)
    {
        if (_Newsize > _capacity)
        {
            _Elem* ptr = new _Elem[_Newsize];
            memset(ptr, 0x00, _Newsize * sizeof(_Elem));
            if (_PreserveContent)
                std::memcpy(ptr, _ptr, _writeIndex * sizeof(_Elem));

            delete[] _ptr;
            _ptr = ptr;
            _capacity = _Newsize;
        }
    }


    bool empty() const
    {
        return (this->_writeIndex == 0);
    }

    void swap(_Myt& _Right)
    {
        if (this == &_Right) return;	// same object, do nothing
        std::swap(this->_ptr,        _Right._ptr);
        std::swap(this->_capacity,   _Right._capacity);
        std::swap(this->_readIndex,  _Right._readIndex);
        std::swap(this->_writeIndex, _Right._writeIndex);
    }

    void move(_Myt&& _Right)
    {
        _Tidy();
        swap(_Right);
    }

    void clear()
    {
        _Tidy();
    }

    _Myt& assign(_Myt&& _Right)
    {
        _Tidy();
        std::swap(this->_ptr, _Right._ptr);
        std::swap(this->_capacity, _Right._capacity);
        std::swap(this->_writeIndex, _Right._writeIndex);
        _Right._Tidy();
        return (*this);
    }

    _Myt& assign(const _Myt& _Right)
    {
        _Tidy();
        _safeNewPtr(_Right.capacity());
        _capacity = _Right.capacity();
        _writeIndex = _Right._writeIndex;
        memcpy(_ptr, _Right._ptr, _Right.capacityBytes());
        return (*this);
    }

    _Myt& assign(const _Elem *_Ptr, size_type _Count)
    {
        _Tidy();
        _safeNewPtr(_Count);
        _capacity = _Count;
        _writeIndex = _Count;
        memcpy(_ptr, _Ptr, _Count*sizeof(_Elem));
        return (*this);
    }

    _Myt& assign(size_type _Count, _Elem _El)
    {
        _Tidy();
        _safeNewPtr(_Count);
        _capacity = _Count;
        for (size_type i = 0; i < _Count; ++i)
            write(_El);

        return (*this);
    }

    _Myt& operator=(_Myt&& _Right)
    {
        return assign(std::forward<_Myt>(_Right));
    }

    _Myt& operator=(const _Myt& _Right)
    {
        return assign(_Right);
    }

    _Myt& operator=(_Elem _Ch)
    {	// assign 1 * _Ch
        return (assign(1, _Ch));
    }


    _Myt& append(const _Myt& _Right)
    {	// append _Right
        return (append(_Right._ptr, _Right.writeSize()));
    }

    _Myt& append(const _Elem *_Ptr, size_type _Count)
    {
        if (_writeIndex + _Count > _capacity)
            resize(_writeIndex + _Count);
        memcpy(_ptr + _writeIndex, _Ptr, _Count*sizeof(_Elem));
        _writeIndex += _Count;
        return (*this);
    }

    _Myt& append(size_type _Count, _Elem _El)
    {
        if (_writeIndex + _Count > _capacity)
            resize(_writeIndex + _Count);
        for (size_type i = 0; i < _Count; ++i)
            write(_El);
        return (*this);
    }

    _Myt& append(_Elem _El)
    {
        return (append((size_type)1, _El));
    }

    _Myt& operator+=(const _Myt& _Right)
    {	// append _Right
        return (append(_Right));
    }

    _Myt& operator+=(_Elem _Ch)
    {	// append 1 * _Ch
        return (append((size_type)1, _Ch));
    }


    // Compare operator.
    bool operator ==(const Buffer& other) const
    {
        if (this != &other)
        {
            if (_writeIndex == other._writeIndex)
            {
                if (std::memcmp(_ptr, other._ptr, _writeIndex * sizeof(_Elem)) == 0)
                {
                    return true;
                }
            }
            return false;
        }
        return true;
    }

    // Compare operator.
    bool operator !=(const Buffer& other) const
    {
        return !(*this == other);
    }

    _Elem& operator [] (size_type index)
    {
        LOG_ASSERT(index < _writeIndex, "Out of range.");
        return _ptr[index];
    }

    const _Elem& operator [] (size_type index) const
    {
        LOG_ASSERT(index < _writeIndex, "Out of range.");
        return _ptr[index];
    }


    // Returns a pointer to the beginning of the buffer.
    _Elem* begin()
    {
        return _ptr;
    }

    // Returns a pointer to the beginning of the buffer.
    const _Elem* begin() const
    {
        return _ptr;
    }

    // Returns a pointer to end of the buffer.
    _Elem* end()
    {
        return _ptr + _writeIndex;
    }

    // Returns a pointer to the end of the buffer.
    const _Elem* end() const
    {
        return _ptr + _writeIndex;
    }

    ~Buffer()
    {
        _Tidy();
    }
protected:
    void _Tidy()
    {
        SAFE_DELETE_ARRAY(_ptr);
        _capacity = 0;
        _readIndex = 0;
        _writeIndex = 0;
    }

    void _safeNewPtr(size_type n)
    {
        _ptr = new _Elem[n];
        memset(_ptr, 0x00, n*sizeof(_Elem));
    }
private:
    _Elem*     _ptr;
    size_type  _capacity;
    size_type  _readIndex;
    size_type  _writeIndex;
};


template <class T = unsigned char>
inline void swap(Buffer<T>& _Left, Buffer<T>& _Right)
{	// swap _Left and _Right strings
    _Left.swap(_Right);
}

NS_FK_END
#endif // LOSEMYMIND_BUFFER_H