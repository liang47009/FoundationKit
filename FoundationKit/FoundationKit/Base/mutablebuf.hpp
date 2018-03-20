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

struct copy_data_to_mutablebuf {};
class basic_mutablebuf
{
public:
    using _Ty = uint8;
    using _Myt = basic_mutablebuf;
    using _Ptr = std::shared_ptr<_Ty>;

    /// Construct an empty buffer.
    basic_mutablebuf()
        : memory_holder(nullptr)
        , memory_length(0)
    {
    }

    basic_mutablebuf(const _Myt& other)
        : memory_holder(other.memory_holder)
        , memory_length(other.memory_length)
    {

    }

    basic_mutablebuf(_Myt&& other)
        : memory_holder(std::move(other.memory_holder))
        , memory_length(other.memory_length)
    {
        other.memory_length = 0;
    }

    /// Construct a buffer to represent a given memory range.
    basic_mutablebuf(uint8* inData, std::size_t length, bool need_del = false)
    {
        assign(inData, length, need_del);
    }

    /// Construct a buffer to represent a given memory range.
    basic_mutablebuf(char* inData, std::size_t length, bool need_del = false)
    {
        assign(reinterpret_cast<uint8*>(inData), length, need_del);
    }

    /// Construct a buffer to represent a given memory range.
    basic_mutablebuf(void* inData, std::size_t length, bool need_del = false)
    {
        assign(reinterpret_cast<uint8*>(inData), length, need_del);
    }

    /// Assignment operator
    basic_mutablebuf& operator= (const _Myt& other)
    {
        memory_length = other.memory_length;
        memory_holder = other.memory_holder;
        return (*this);
    }

    basic_mutablebuf& operator= (_Myt&& other)
    {
        memory_length = other.memory_length;
        memory_holder = std::move(other.memory_holder);
        other.memory_length = 0;
        return (*this);
    }

    _Ty* data()
    {
        return memory_holder.get();
    }

    const _Ty * data() const
    {
        return memory_holder.get();
    }

    /// Get the size of the memory range.
    size_t size() const
    {
        return memory_length;
    }

    const char* c_str()const
    {
        return reinterpret_cast<const char*>(data());
    }

    char* str()
    {
        return reinterpret_cast<char*>(data());
    }

    bool empty()
    {
        return !memory_holder || memory_length == 0;
    }


    void assign(uint8* inData, std::size_t length, bool need_del = false)
    {
        if (need_del)
        {
            memory_holder = _Ptr(inData);
        }
        else
        {
            memory_holder = _Ptr(inData, [&](_Ty* /*ptr*/) {/*do nothing...*/});
        }
        memory_length = length;
    }

    ~basic_mutablebuf()
    {

    }
protected:
    _Ptr            memory_holder;
    size_t          memory_length;
};

typedef basic_mutablebuf mutablebuf;


inline mutablebuf make_mutablebuf(std::vector<char>& buffers)
{
	return mutablebuf(buffers.data(), buffers.size());
}

inline mutablebuf make_mutablebuf(std::vector<unsigned char>& buffers)
{
    return mutablebuf(buffers.data(), buffers.size());
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
    constbuf(const uint8* inData, std::size_t length)
        : _data(inData)
        , _size(length)
    {
    }

    /// Construct a buffer to represent a given memory range.
    constbuf(const char* inData, std::size_t length)
        : _data(inData)
        , _size(length)
    {
    }

    /// Construct a buffer to represent a given memory range.
    constbuf(const void* inData, std::size_t length)
        : _data(inData)
        , _size(length)
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

    void assign(void* inData, std::size_t length)
    {
        clear();
        _data = inData;
        _size = length;
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
