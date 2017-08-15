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
#include "FoundationKit/Base/types.hpp"

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

template <typename Allocator = std::allocator<uint8> >
class basic_mutablebuf
{
public:
    typedef basic_mutablebuf<Allocator> _Myt;

    /// Construct an empty buffer.
    basic_mutablebuf()
        : _data(nullptr)
        , _size(0)
        , _owner(false)
    {
    }

    basic_mutablebuf(const _Myt& other)
        : _data(nullptr)
        , _size(0)
        , _owner(false)
    {
        copy(other);
    }

    basic_mutablebuf(_Myt&& other)
        : _data(nullptr)
        , _size(0)
        , _owner(false)
    {
        move(std::forward<_Myt&&>(other));
    }

	basic_mutablebuf(std::size_t size)
		: _data(nullptr)
		, _size(0)
		, _owner(false)
	{
		_data = new uint8[size];
		memset(_data, 0, size);
		_size = size;
		_owner = true;
	}

    /// Construct a buffer to represent a given memory range.
    basic_mutablebuf(uint8* data, std::size_t size, bool need_del = false)
        : _data(data)
        , _size(size)
        , _owner(need_del)
    {
    }

    /// Construct a buffer to represent a given memory range.
    basic_mutablebuf(char* data, std::size_t size, bool need_del = false)
        : _data(reinterpret_cast<uint8*>(data))
        , _size(size)
        , _owner(need_del)
    {
    }

    /// Construct a buffer to represent a given memory range.
    basic_mutablebuf(void* data, std::size_t size, bool need_del = false)
        : _data(reinterpret_cast<uint8*>(data))
        , _size(size)
        , _owner(need_del)
    {
    }

    /// Assignment operator
    basic_mutablebuf& operator= (const _Myt& other)
    {
        if (this != &other)
        {
            copy(other);
        }
        return *this;
    }

    basic_mutablebuf& operator= (_Myt&& other)
    {
        if (this != &other)
        {
            move(std::forward<_Myt&&>(other));
        }
        return *this;
    }

    /// Get a pointer to the beginning of the memory range.
    void* data() const
    {
        return reinterpret_cast<void*>(_data);;
    }

    /// Get the size of the memory range.
    std::size_t size() const
    {
        return _size;
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
        release();
        _data = new uint8[size];
        memset(_data, 0, size);
        _size = size;
        _owner = true;
    }

    void assign(void* data, std::size_t size, bool need_del = false)
    {
        release();
        _data = reinterpret_cast<uint8*>(data);
        _size = size;
        _owner = need_del;
    }

    void clear()
    {
        memset(_data, 0, _size);
    }

    void release()
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

    ~basic_mutablebuf()
    {
        release();
    }

private:
    void copy(const _Myt& other)
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

    void move(_Myt&& other)
    {
        release();
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
    //Allocator   _allocator;
};

typedef basic_mutablebuf<> mutablebuf;

inline mutablebuf make_mutablebuf(std::vector<char>& buffers)
{
    return mutablebuf(&(buffers[0]), buffers.size());
}

inline mutablebuf make_mutablebuf(std::vector<unsigned char>& buffers)
{
    return mutablebuf(&(buffers[0]), buffers.size());
}

inline mutablebuf make_mutablebuf(std::basic_string<char>& buffers)
{
    return mutablebuf(&(buffers[0]), buffers.size());
}

inline mutablebuf make_mutablebuf(std::basic_string<unsigned char>& buffers)
{
    return mutablebuf(&(buffers[0]), buffers.size());
}


/**
 * Holds a buffer that cannot be modified.
 * The constbuf class provides a safe representation of a buffer that cannot
 * be modified. It does not own the underlying data, and so is cheap to copy or
 * assign.
 *
 * @code constbuf b1 = ...;
 * std::size_t s1 = b1.size();
 * const unsigned char* p1 = static_cast<const unsigned char*>(b1.data());
 * @endcode
 *
 * The data() function permits violations of type safety, so
 * uses of it in application code should be carefully considered.
 */
class constbuf
{
public:

    /// Construct an empty buffer.
    constbuf()
        : _data(nullptr)
        , _size(0)
    {
    }


    constbuf(const constbuf& other)
    {
        copy(other);
    }

    constbuf(constbuf&& other)
    {
        move(std::forward<constbuf&&>(other));
    }

    constbuf(const mutablebuf& mutableBuf)
    {
        this->_size = mutableBuf.size();
        this->_data = mutableBuf.data();
    }

    /// Construct a buffer to represent a given memory range.
    constbuf(const uint8* data, std::size_t size)
        : _data(data)
        , _size(size)
    {
    }

    /// Construct a buffer to represent a given memory range.
    constbuf(const char* data, std::size_t size)
        : _data(data)
        , _size(size)
    {
    }

    /// Construct a buffer to represent a given memory range.
    constbuf(const void* data, std::size_t size)
        : _data(data)
        , _size(size)
    {
    }

    // Assignment operator
    constbuf& operator= (const constbuf& other)
    {
        if (this != &other)
        {
            copy(other);
        }
        return *this;
    }

    constbuf& operator= (constbuf&& other)
    {
        if (this != &other)
        {
            move(std::forward<constbuf&&>(other));
        }
        return *this;
    }

    /// Get a pointer to the beginning of the memory range.
    const void* data() const
    {
        return _data;
    }

    /// Get the size of the memory range.
    std::size_t size() const
    {
        return _size;
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

    void clear()
    {
        _data = nullptr;
        _size = 0;
    }

    bool empty()
    {
        return (_data == nullptr || _size == 0);
    }

    ~constbuf()
    {
        clear();
    }

private:
    void copy(const constbuf& other)
    {
        this->_size = other._size;
        this->_data = other._data;
    }

    void move(constbuf&& other)
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

inline constbuf make_constbuf(std::vector<char>& buffers)
{
    return constbuf(&(buffers[0]), buffers.size());
}

inline constbuf make_constbuf(std::vector<unsigned char>& buffers)
{
    return constbuf(&(buffers[0]), buffers.size());
}

inline constbuf make_constbuf(std::basic_string<char>& buffers)
{
    return constbuf(&(buffers[0]), buffers.size());
}

inline constbuf make_constbuf(std::basic_string<unsigned char>& buffers)
{
    return constbuf(&(buffers[0]), buffers.size());
}

NS_FK_END

#endif // FOUNDATIONKIT_MULTIPLE_BUFFER_HPP
