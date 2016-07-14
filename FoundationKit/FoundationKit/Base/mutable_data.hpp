#ifndef LOSEMYMIND_MUTABLE_DATA_HPP
#define LOSEMYMIND_MUTABLE_DATA_HPP

#include "FoundationKit/GenericPlatformMacros.h"

#include <type_traits>
#include <string>
#include <vector>

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

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
* std::size_t s1 = b1.size();
* unsigned char* p1 = static_cast<unsigned char*>(b1.data());
* @endcode
*
* The @c data() member function permits violations of type safety, so uses of
* it in application code should be carefully considered.
*/
class mutable_data
{
public:

    /// Construct an empty buffer.
    mutable_data()
        : data_(nullptr)
        , size_(0)
    {
    }

    mutable_data(const mutable_data& other)
    {
        copy(other);
    }

    mutable_data(mutable_data&& other)
    {
        move(std::forward<mutable_data&&>(other));
    }

    mutable_data(char* data, std::size_t size, bool need_del = false)
        : data_(static_cast<void*>(data))
        , size_(size)
        , owner_(need_del)
    {
    }

    /// Construct a buffer to represent a given memory range.
    mutable_data(void* data, std::size_t size, bool need_del = false)
        : data_(data)
        , size_(size)
        , owner_(need_del)
    {
    }

    // Assignment operator
    mutable_data& operator= (const mutable_data& other)
    {
        copy(other);
    }

    mutable_data& operator= (mutable_data&& other)
    {
        move(std::forward<mutable_data&&>(other));
    }

    /// Get a pointer to the beginning of the memory range.
    void* data() const
    {
        return data_;
    }

    const char* c_str()
    {
        return static_cast<const char*>(data_);
    }

    void reserve(std::size_t size)
    {
        clear();
        char* pBuf = new char[size];
        data_ = pBuf;
        size_ = size;
        owner_ = true;
    }

    void assign(void* data, std::size_t size, bool need_del = false)
    {
        clear();
        data_ = data;
        size_ = size;
        owner_ = need_del;
    }

    /// Get the size of the memory range.
    std::size_t size() const
    {
        return size_;
    }

    void clear()
    {
        if (owner_ && data_ != nullptr)
        {
            delete[] data_;
        }
        data_ = nullptr;
        size_ = 0;
        owner_ = false;
    }

    ~mutable_data()
    {
        clear();
    }

private:
    void copy(const mutable_data& other)
    {
        this->size_ = other.size_;
        if (!other.owner_)
        {
            this->data_ = other.data_;
        }
        else
        {
            if (this->data_ != nullptr && owner_ == true)
            {
                delete data_;
            }
            data_ = new char[other.size_];
            memcpy(data_, other.data_, other.size_);
        }
    }

    void move(mutable_data&& other)
    {
        this->data_ = other.data_;
        this->size_ = other.size_;
        this->owner_ = other.owner_;
        other.data_ = nullptr;
        other.size_ = 0;
        other.owner_ = false;
    }

private:
    void* data_;
    std::size_t size_;
    bool owner_;
};

inline mutable_data make_mutable_data(std::vector<char>& buffers)
{
    return std::move(mutable_data(&(buffers[0]), buffers.size()));
}

inline mutable_data make_mutable_data(std::vector<unsigned char>& buffers)
{
    return std::move(mutable_data(&(buffers[0]), buffers.size()));
}

inline mutable_data make_mutable_data(std::basic_string<char>& buffers)
{
    return std::move(mutable_data(&(buffers[0]), buffers.size()));
}

inline mutable_data make_mutable_data(std::basic_string<unsigned char>& buffers)
{
    return std::move(mutable_data(&(buffers[0]), buffers.size()));
}

NS_FK_END

#endif // LOSEMYMIND_MUTABLE_DATA_HPP
