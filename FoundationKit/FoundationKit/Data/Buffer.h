/****************************************************************************
  Copyright (c) 2013-2014 libo.
 
  losemymind.libo@gmail.com

****************************************************************************/
#pragma once

#include <cstring>
#include <cstddef>
#include <memory>
#include "FoundationKit/Foundation/Exception.h"

NS_FK_BEGIN

/** 
 * A buffer class that allocates a buffer of a given type and size 
 * in the constructor and deallocates the buffer in the destructor.
 *
 * This class is useful everywhere where a temporary buffer
 * is needed.
 */
template <class T = unsigned char>
class Buffer
{
 /** Creates and allocates the Buffer. */
public:
    // default capacity is 8 byte.
    Buffer():
        _capacity(8),
        _used(0),
        _ptr(new T[_capacity]),
        _ownMem(true)
    {
        memset(_ptr, 0, _capacity);
    }

    Buffer(std::size_t capacity):
        _capacity(capacity),
        _used(0),
        _ptr(nullptr),
        _ownMem(true)
    {
        if (capacity > 0)
        {
            _ptr = new T[capacity];
            memset(_ptr, 0, _capacity);
        }
    }

    /** 
     * Creates the Buffer. Length argument specifies the length
     * of the supplied memory pointed to by pMem in the number
     * of elements of type T. Supplied pointer is considered
     * blank and not owned by Buffer, so in this case Buffer 
     * only acts as a wrapper around externally supplied 
     * (and lifetime-managed) memory.
     */
    explicit Buffer(T* pMem, std::size_t length):
        _capacity(length),
        _used(length),
        _ptr(pMem),
        _ownMem(false)
    {
    }

    /** 
     * Creates and allocates the Buffer; copies the contents of
     * the supplied memory into the buffer. Length argument specifies
     * the length of the supplied memory pointed to by pMem in the
     * number of elements of type T.
     */
    Buffer(const T* pMem, std::size_t length):
        _capacity(length),
        _used(length),
        _ptr(nullptr),
        _ownMem(true)
    {
        if (_used > 0)
        {
            _ptr = new T[_capacity];
            std::memcpy(_ptr, pMem, _used * sizeof(T));
        }
    }

    /**  Copy constructor. */
    Buffer(const Buffer& other):
        _capacity(other._used),
        _used(other._used),
        _ptr(nullptr),
        _ownMem(true)
    {
        if (_used)
        {
            _ptr = new T[_used];
            std::memcpy(_ptr, other._ptr, _used * sizeof(T));
        }
    }

    /** Assignment operator. */
    Buffer& operator =(const Buffer& other)
    {
        if (this != &other)
        {
            Buffer tmp(other);
            swap(tmp);
            _ownMem = true;
        }
        return *this;
    }

    ~Buffer()
    {
        if (_ownMem) delete [] _ptr;
    }

    /** 
     * Resizes the buffer capacity and size. If preserveContent is true,
     * the content of the old buffer is copied over to the
     * new buffer. The new capacity can be larger or smaller than
     * the current one; if it is smaller, capacity will remain intact.
     * Size will always be set to the new capacity.
     *  
     * Buffers only wrapping externally owned storage can not be 
     * resized. If resize is attempted on those, IllegalAccessException
     * is thrown.
     */
    void resize(std::size_t newCapacity, bool preserveContent = true)
    {
        if (!_ownMem)
            throw InvalidAccessException("Cannot resize buffer which does not own its storage.");

        if (newCapacity > _capacity)
        {
            T* ptr = new T[newCapacity];
            if (preserveContent)
                std::memcpy(ptr, _ptr, _used * sizeof(T));

            delete [] _ptr;
            _ptr = ptr;
            _capacity = newCapacity;
        }

        //_used = newCapacity;
    }

    /** 
     * Sets the buffer capacity. If preserveContent is true,
     * the content of the old buffer is copied over to the
     * new buffer. The new capacity can be larger or smaller than
     * the current one; size will be set to the new capacity only if 
     * new capacity is smaller than the current size, otherwise it will
     * remain intact.
     * 
     * Buffers only wrapping externally owned storage can not be 
     * resized. If resize is attempted on those, IllegalAccessException
     * is thrown.
     */
    void setCapacity(std::size_t newCapacity, bool preserveContent = true)
    {
        if (!_ownMem)
            throw InvalidAccessException("Cannot resize buffer which does not own its storage.");

        if (newCapacity != _capacity)
        {
            T* ptr = nullptr;
            if (newCapacity > 0)
            {
                ptr = new T[newCapacity];
                if (preserveContent)
                {
                    std::size_t newSz = _used < newCapacity ? _used : newCapacity;
                    std::memcpy(ptr, _ptr, newSz * sizeof(T));
                }
            }

            delete [] _ptr;
            _ptr = ptr;
            _capacity = newCapacity;

            if (newCapacity < _used) _used = newCapacity;
        }
    }
    /** 
     * Assigns the argument buffer to this buffer.
     * If necessary, resizes the buffer.
     */
    void assign(const T* buf, std::size_t sz)
    {
        if (0 == sz) return;
        if (sz > _capacity) 
            resize(sz, false);
        std::memcpy(_ptr, buf, sz * sizeof(T));
        _used = sz;
    }

    /** Resizes this buffer and appends the argument buffer.*/
    const T* append(const T* buf, std::size_t sz)
    {
        if (0 == sz) return 0;
        resize(_used + sz, true);
        std::memcpy(_ptr + _used, buf, sz * sizeof(T));
        _used += sz;
        return (_ptr + _used);
    }

    /** Resizes this buffer by one element and appends the argument value. */
    const T* append(T val)
    {
        resize(_used + sizeof(T), true);
        _ptr[_used] = val;
        _used += sizeof(T);
        return (_ptr + _used);
    }

    // Resizes this buffer and appends the argument buffer.
    const T* append(const Buffer& buf)
    {
        return append(buf.begin(), buf.size());
    }

    // Returns the allocated memory size in elements.
    std::size_t capacity() const 
    {
        return _capacity;
    }

    // Returns the allocated memory size in bytes.
    std::size_t capacityBytes() const 
    {
        return _capacity * sizeof(T);
    }

    // Swaps the buffer with another one.
    void swap(Buffer& other) 
    {
        using std::swap;
        swap(_ptr, other._ptr);
        swap(_capacity, other._capacity);
        swap(_used, other._used);
    }

    // Compare operator.
    bool operator ==(const Buffer& other) const
    {
        if (this != &other)
        {
            if (_used == other._used)
            {
                if (std::memcmp(_ptr, other._ptr, _used * sizeof(T)) == 0)
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

    // Sets the contents of the bufer to zero.
    void clear()
    {
        std::memset(_ptr, 0, _used * sizeof(T));
        _used = 0£»
    }

    // Returns the used size of the buffer in elements.
    std::size_t size() const
    {
        return _used;
    }

    // Returns the used size of the buffer in bytes.
    std::size_t sizeBytes() const  
    {
        return _used * sizeof(T);
    }

    // Returns a pointer to the beginning of the buffer.
    T* begin()
    {
        return _ptr;
    }

    // Returns a pointer to the beginning of the buffer.
    const T* begin() const
    {
        return _ptr;
    }

    // Returns a pointer to end of the buffer.
    T* end()
    {
        return _ptr + _used;
    }

    // Returns a pointer to the end of the buffer.
    const T* end() const
    {
        return _ptr + _used;
    }

    // Return true if buffer is empty.
    bool empty() const
    {
        return 0 == _used;
    }

    T& operator [] (std::size_t index)
    {
        assert (index < _used);
        return _ptr[index];
    }

    const T& operator [] (std::size_t index) const
    {
        assert (index < _used);
        return _ptr[index];
    }

private:
    std::size_t _capacity;
    std::size_t _used;
    T*          _ptr;
    bool        _ownMem;
};

NS_FK_END

