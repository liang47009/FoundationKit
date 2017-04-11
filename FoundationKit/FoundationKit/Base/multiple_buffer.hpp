/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_MULTIPLE_BUFFER_HPP
#define FOUNDATIONKIT_MULTIPLE_BUFFER_HPP


#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <string>
#include <vector>
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Base/Types.hpp"

NS_FK_BEGIN

/**
* Holds a buffer that can be modified.
* The mutable_buffer class provides a safe representation of a buffer that can
* be modified. It does not own the underlying data, and so is cheap to copy or
* assign.
*
* @par Accessing Buffer Contents
*
* The contents of a buffer may be accessed using the @c data() and @c size()
* member functions:
*
* @code mutable_buffer b1 = ...;
* std::_sizet s1 = b1.size();
* unsigned char* p1 = static_cast<unsigned char*>(b1.data());
* @endcode
*
* The @c data() member function permits violations of type safety, so uses of
* it in application code should be carefully considered.
*/
class mutable_buffer
{
public:

    typedef uint8* pointer;

    /// Construct an empty buffer.
    mutable_buffer()
        : _data(nullptr)
        , _size(0)
    {
    }

    mutable_buffer(const mutable_buffer& other)
    {
        copy(other);
    }

    mutable_buffer(mutable_buffer&& other)
    {
        move(std::forward<mutable_buffer&&>(other));
    }

    /// Construct a buffer to represent a given memory range.
    mutable_buffer(uint8* data, std::size_t size, bool need_del = false)
        : _data(data)
        , _size(size)
        , _owner(need_del)
    {
    }

    /// Construct a buffer to represent a given memory range.
    mutable_buffer(char* data, std::size_t size, bool need_del = false)
        : _data(reinterpret_cast<uint8*>(data))
        , _size(size)
        , _owner(need_del)
    {
    }

    /// Construct a buffer to represent a given memory range.
    mutable_buffer(void* data, std::size_t size, bool need_del = false)
        : _data(reinterpret_cast<uint8*>(data))
        , _size(size)
        , _owner(need_del)
    {
    }

    /// Assignment operator
    mutable_buffer& operator= (const mutable_buffer& other)
    {
        if (this != &other)
        {
            copy(other);
        }
        return *this;
    }

    mutable_buffer& operator= (mutable_buffer&& other)
    {
        if (this != &other)
        {
            move(std::forward<mutable_buffer&&>(other));
        }
        return *this;
    }

    /// Get a pointer to the beginning of the memory range.
    void* data() const
    {
        return reinterpret_cast<void*>(_data);;
    }

    const char* c_str()const
    {
        return reinterpret_cast<const char*>(_data);
    }

    const uint8* uc_str()
    {
        return _data;
    }

    void reallocate(std::size_t size)
    {
        clear();
        _data = new uint8[size];
        memset(_data, 0, size);
        _size = size;
        _owner = true;
    }

    void assign(void* data, std::size_t size, bool need_del = false)
    {
        clear();
        _data = reinterpret_cast<uint8*>(data);
        _size = size;
        _owner = need_del;
    }

    /// Get the size of the memory range.
    std::size_t size() const
    {
        return _size;
    }

    void clear()
    {
        if (_owner && _data != nullptr)
        {
            delete[] _data;
        }
        _data = nullptr;
        _size = 0;
        _owner = false;
    }

    bool empty()
    {
        return (_data == nullptr || _size == 0);
    }

    ~mutable_buffer()
    {
        clear();
    }

private:
    void copy(const mutable_buffer& other)
    {
        this->_size = other._size;
        if (!other._owner)
        {
            this->_data = other._data;
            this->_owner = other._owner;
        }
        else
        {
            if (this->_data != nullptr && _owner == true)
            {
                delete[] _data;
            }
            _data = new uint8[other._size];
            memset(_data, 0, other._size);
            memcpy(_data, other._data, other._size);
            this->_owner = true;
        }
    }

    void move(mutable_buffer&& other)
    {
        clear();
        this->_data = other._data;
        this->_size = other._size;
        this->_owner = other._owner;
        other._data = nullptr;
        other._size = 0;
        other._owner = false;
    }

private:
    uint8*      _data;
    std::size_t _size;
    bool        _owner;
};

inline mutable_buffer make_mutable_buffer(std::vector<char>& buffers)
{
    return mutable_buffer(&(buffers[0]), buffers.size());
}

inline mutable_buffer make_mutable_buffer(std::vector<unsigned char>& buffers)
{
    return mutable_buffer(&(buffers[0]), buffers.size());
}

inline mutable_buffer make_mutable_buffer(std::basic_string<char>& buffers)
{
    return mutable_buffer(&(buffers[0]), buffers.size());
}

inline mutable_buffer make_mutable_buffer(std::basic_string<unsigned char>& buffers)
{
    return mutable_buffer(&(buffers[0]), buffers.size());
}


/**
 * Holds a buffer that cannot be modified.
 * The const_buffer class provides a safe representation of a buffer that cannot
 * be modified. It does not own the underlying data, and so is cheap to copy or
 * assign.
 *
 * @par Accessing Buffer Contents
 *
 * The contents of a buffer may be accessed using the @ref buffer_size
 * and @ref buffer_cast functions:
 *
 * @code const_buffer b1 = ...;
 * std::size_t s1 = b1.size();
 * const unsigned char* p1 = static_cast<const unsigned char*>(b1.data());
 * @endcode
 *
 * The data() function permits violations of type safety, so
 * uses of it in application code should be carefully considered.
 */
class const_buffer
{
public:

    typedef void* pointer;

    /// Construct an empty buffer.
    const_buffer()
        : _data(nullptr)
        , _size(0)
    {
    }


    const_buffer(const const_buffer& other)
    {
        copy(other);
    }

    const_buffer(const_buffer&& other)
    {
        move(std::forward<const_buffer&&>(other));
    }

    const_buffer(const mutable_buffer& mutableBuf)
    {
        this->_size = mutableBuf.size();
        this->_data = mutableBuf.data();
    }

    /// Construct a buffer to represent a given memory range.
    const_buffer(const uint8* data, std::size_t size)
        : _data(data)
        , _size(size)
    {
    }

    /// Construct a buffer to represent a given memory range.
    const_buffer(const char* data, std::size_t size)
        : _data(data)
        , _size(size)
    {
    }

    /// Construct a buffer to represent a given memory range.
    const_buffer(const void* data, std::size_t size)
        : _data(data)
        , _size(size)
    {
    }

    // Assignment operator
    const_buffer& operator= (const const_buffer& other)
    {
        if (this != &other)
        {
            copy(other);
        }
        return *this;
    }

    const_buffer& operator= (const_buffer&& other)
    {
        if (this != &other)
        {
            move(std::forward<const_buffer&&>(other));
        }
        return *this;
    }

    /// Get a pointer to the beginning of the memory range.
    const void* data() const
    {
        return _data;
    }

    const char* c_str()const
    {
        return reinterpret_cast<const char*>(_data);
    }

    const uint8* uc_str()
    {
        return reinterpret_cast<const uint8*>(_data);
    }

    void assign(void* data, std::size_t size)
    {
        clear();
        _data = data;
        _size = size;
    }

    /// Get the size of the memory range.
    std::size_t size() const
    {
        return _size;
    }

    void clear()
    {
        _data = nullptr;
        _size = 0;
    }

    bool empty()
    {
        return (_data == nullptr || _size == 0);
    }

    ~const_buffer()
    {
        clear();
    }

private:
    void copy(const const_buffer& other)
    {
        this->_size = other._size;
        this->_data = other._data;
    }

    void move(const_buffer&& other)
    {
        this->_data = other._data;
        this->_size = other._size;
        other._data = nullptr;
        other._size = 0;
    }

private:
    const void* _data;
    std::size_t _size;
};

inline const_buffer make_const_buffer(std::vector<char>& buffers)
{
    return const_buffer(&(buffers[0]), buffers.size());
}

inline const_buffer make_const_buffer(std::vector<unsigned char>& buffers)
{
    return const_buffer(&(buffers[0]), buffers.size());
}

inline const_buffer make_const_buffer(std::basic_string<char>& buffers)
{
    return const_buffer(&(buffers[0]), buffers.size());
}

inline const_buffer make_const_buffer(std::basic_string<unsigned char>& buffers)
{
    return const_buffer(&(buffers[0]), buffers.size());
}

NS_FK_END

#endif // FOUNDATIONKIT_MULTIPLE_BUFFER_HPP
