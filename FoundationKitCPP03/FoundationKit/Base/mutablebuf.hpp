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
#include <memory>
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

class basic_mutablebuf
{
public:
	typedef uint8 value_type;
	typedef size_t size_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef basic_mutablebuf _Myt;

    /// Construct an empty buffer.
    basic_mutablebuf()
		: _Ptr()
		, _Mysize()
        , _bAlloced(false)
    {
    }

    basic_mutablebuf(const _Myt& other)
		: _Ptr()
		, _Mysize()
		, _bAlloced(false)
    {
        copy(other);
    }

	basic_mutablebuf(std::size_t size)
		: _Ptr()
		, _Mysize()
		, _bAlloced(false)
	{
		allocate(size);
	}

    /// Construct a buffer to represent a given memory range.
    basic_mutablebuf(uint8* data, std::size_t size, bool need_del = false)
		: _Ptr(data)
		, _Mysize(size)
		, _bAlloced(need_del)
    {
    }

    /// Construct a buffer to represent a given memory range.
    basic_mutablebuf(char* data, std::size_t size, bool need_del = false)
        : _Ptr(reinterpret_cast<uint8*>(data))
        , _Mysize(size)
        , _bAlloced(need_del)
    {
    }

    /// Construct a buffer to represent a given memory range.
    basic_mutablebuf(void* data, std::size_t size, bool need_del = false)
        : _Ptr(reinterpret_cast<uint8*>(data))
        , _Mysize(size)
        , _bAlloced(need_del)
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

    /// Get a pointer to the beginning of the memory range.
    void* data() const
    {
        return reinterpret_cast<void*>(_Ptr);;
    }

    /// Get the size of the memory range.
	size_type size() const
    {
        return _Mysize;
    }

    const char* c_str()const
    {
        return reinterpret_cast<const char*>(_Ptr);
    }

	const_pointer uc_str()
    {
        return _Ptr;
    }

	void allocate(const size_type size)
	{
		_Ptr = new value_type[size];
		_Mysize = size;
		_bAlloced = true;
		memset(_Ptr, 0, _Mysize);
	}

	void deallocate()
	{
		if (_bAlloced && _Ptr != nullptr)
			delete[] _Ptr;
		_Ptr = pointer();
		_Mysize = 0;
		_bAlloced = false;
	}


    void reallocate(std::size_t size)
    {
		deallocate();
		allocate(size);
    }

    void assign(void* data, std::size_t size, bool need_del = false)
    {
		deallocate();
		_Ptr = reinterpret_cast<uint8*>(data);
		_Mysize = size;
		_bAlloced = need_del;
    }

	void clear()
	{
		memset(_Ptr, 0, _Mysize);
	}

    bool empty()
    {
        return (_Ptr == nullptr || _Mysize == 0);
    }

    ~basic_mutablebuf()
    {
		deallocate();
    }

	void copy(const _Myt& other)
	{
		if (!other._bAlloced)
		{
			assign(other._Ptr, other._Mysize);
		}
		else
		{
			copy(other._Ptr, other._Mysize);
		}
	}

	void copy(void* data, std::size_t size)
	{
		deallocate();
		allocate(size);
		memcpy(this->data(), data, size);
	}

private:
	pointer   _Ptr;
	size_type _Mysize;	// current length of string
	bool      _bAlloced;
};

typedef basic_mutablebuf mutablebuf;

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
