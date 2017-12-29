/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_BASIC_STREAMBUF_HPP
#define FOUNDATIONKIT_BASIC_STREAMBUF_HPP

#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <streambuf>
#include <vector>
#include "FoundationKit/Base/mutablebuf.hpp"
#include "FoundationKit/Base/noncopyable.hpp"

NS_FK_BEGIN

/**
 * Automatically resizable buffer class based on std::streambuf.
 * The @c basic_streambuf class is derived from @c std::streambuf to associate
 * the streambuf's input and output sequences with one or more character
 * arrays. These character arrays are internal to the @c basic_streambuf
 * object, but direct access to the array elements is provided to permit them
 * to be used efficiently with I/O operations. Characters written to the output
 * sequence of a @c basic_streambuf object are appended to the input sequence
 * of the same object.
 *
 * The @c basic_streambuf class's public interface is intended to permit the
 * following implementation strategies:
 *
 * @li A single contiguous character array, which is reallocated as necessary
 * to accommodate changes in the size of the character sequence. 
 *
 * @li A sequence of one or more character arrays, where each array is of the
 * same size. Additional character array objects are appended to the sequence
 * to accommodate changes in the size of the character sequence.
 *
 * @li A sequence of one or more character arrays of varying sizes. Additional
 * character array objects are appended to the sequence to accommodate changes
 * in the size of the character sequence.
 *
 * The constructor for basic_streambuf accepts a @c size_t argument specifying
 * the maximum of the sum of the sizes of the input sequence and output
 * sequence. During the lifetime of the @c basic_streambuf object, the following
 * invariant holds:
 * @code size() <= max_size()@endcode
 * Any member function that would, if successful, cause the invariant to be
 * violated shall throw an exception of class @c std::length_error.
 *
 * The constructor for @c basic_streambuf takes an Allocator argument. A copy
 * of this argument is used for any memory allocation performed, by the
 * constructor and by all member functions, during the lifetime of each @c
 * basic_streambuf object.
 *
 * @par Examples
 * Writing directly from an streambuf to a socket:
 * @code
 * streambuf b;
 * std::ostream os(&b);
 * os << "Hello, World!\n";
 *
 * // try sending some data in input sequence
 * size_t n = sock.send(b.data());
 *
 * b.consume(n); // sent data is removed from input sequence
 * @endcode
 *
 * Reading from a socket directly into a streambuf:
 * @code
 * streambuf b;
 *
 * // reserve 512 bytes in output sequence
 * streambuf::mutable_buffers_type bufs = b.prepare(512);
 *
 * size_t n = sock.receive(bufs);
 *
 * // received data is "committed" from output sequence to input sequence
 * b.commit(n);
 *
 * std::istream is(&b);
 * std::string s;
 * is >> s;
 * @endcode
 */

 // https://msdn.microsoft.com/zh-cn/library/9h24a8cd.aspx
 // http://en.cppreference.com/w/cpp/io/basic_streambuf
template <typename Allocator = std::allocator<char> >
class basic_streambuf : public std::streambuf, private noncopyable
{
public:
    typedef mutablebuf mutablebuf_type;

    typedef constbuf   constbuf_type;
    /**
     * Construct a basic_streambuf object.
     * Constructs a streambuf with the specified maximum size. The initial size
     * of the streambuf's input sequence is 0.
     */
    explicit basic_streambuf(std::size_t maximum_size = (std::numeric_limits<std::size_t>::max)(),const Allocator& allocator = Allocator())
        : _max_size(maximum_size)
        , _buffer(allocator)
    {
        std::size_t pend = (std::min<std::size_t>)(_max_size, _bufferdelta);
        _buffer.resize((std::max<std::size_t>)(pend, 1));
        setg(&_buffer[0], &_buffer[0], &_buffer[0]);
        setp(&_buffer[0], &_buffer[0] + pend);
    }

    /**
     * Get the size of the input sequence.
     * @returns The size of the input sequence.
     */
    std::size_t size() const
    {
        return pptr() - gptr();
    }

    /**
     * Get the maximum size of the basic_streambuf.
     * @returns The allowed maximum of the sum of the sizes of the input sequence
     * and output sequence.
     */
    std::size_t max_size() const
    {
        return _max_size;
    }

    /**
     * Get the current capacity of the basic_streambuf.
     * @returns The current total capacity of the streambuf, i.e. for both the
     * input sequence and output sequence.
     */
    std::size_t capacity() const
    {
        return _buffer.capacity();
    }

    /**
     * Get a list of buffers that represents the input sequence.
     * @returns An object of type @c const_buffers_type that satisfies
     * ConstBufferSequence requirements, representing all character arrays in the
     * input sequence.
     *
     * @note The returned object is invalidated by any @c basic_streambuf member
     * function that modifies the input sequence or output sequence.
     */
    constbuf_type data() const
    {
        return constbuf_type(gptr(), (pptr() - gptr()) * sizeof(char_type));
    }

    /**
     * Get a list of buffers that represents the output sequence, with the given
     * size.
     * Ensures that the output sequence can accommodate @c n characters,
     * reallocating character array objects as necessary.
     *
     * @returns An object of type @c mutable_buffer that satisfies
     * MutableBufferSequence requirements, representing character array objects
     * at the start of the output sequence such that the sum of the buffer sizes
     * is @c n.
     *
     * @throws std::length_error If <tt>size() + n > max_size()</tt>.
     *
     * @note The returned object is invalidated by any @c basic_streambuf member
     * function that modifies the input sequence or output sequence.
     */
    mutablebuf_type prepare(std::size_t n)
    {
        reserve(n);
        return mutablebuf_type(pptr(), n * sizeof(char_type));
    }

    /**
     * Move characters from the output sequence to the input sequence.
     * Appends @c n characters from the start of the output sequence to the input
     * sequence. The beginning of the output sequence is advanced by @c n
     * characters.
     *
     * Requires a preceding call <tt>prepare(x)</tt> where <tt>x >= n</tt>, and
     * no intervening operations that modify the input or output sequence.
     *
     * @note If @c n is greater than the size of the output sequence, the entire
     * output sequence is moved to the input sequence and no error is issued.
     */
    void commit(std::size_t n)
    {
        if (pptr() + n > epptr())
            n = epptr() - pptr();
        pbump(static_cast<int>(n));
        setg(eback(), gptr(), pptr());
    }

    /**
     * Remove characters from the input sequence.
     * Removes @c n characters from the beginning of the input sequence.
     *
     * @note If @c n is greater than the size of the input sequence, the entire
     * input sequence is consumed and no error is issued.
     */
    void consume(std::size_t n)
    {
        if (egptr() < pptr())
            setg(&_buffer[0], gptr(), pptr());
        if (gptr() + n > pptr())
            n = pptr() - gptr();
        gbump(static_cast<int>(n));
    }

    void reset()
    {
        std::size_t pend = (std::min<std::size_t>)(_max_size, _bufferdelta);
        _buffer.resize((std::max<std::size_t>)(pend, 1));
        _buffer.assign(_buffer.size(), '\0');
        setg(&_buffer[0], &_buffer[0], &_buffer[0]);
        setp(&_buffer[0], &_buffer[0] + pend);
    }

protected:
    enum { _bufferdelta = 128 };

    /**
     * Override std::streambuf behaviour.
     * Behaves according to the specification of @c std::streambuf::underflow().
     */
    int_type underflow()
    {
        if (gptr() < pptr())
        {
            setg(&_buffer[0], gptr(), pptr());
            return traits_type::to_int_type(*gptr());
        }
        else
        {
            return traits_type::eof();
        }
    }

    /**
     * Override std::streambuf behaviour.
     * Behaves according to the specification of @c std::streambuf::overflow(),
     * with the specialisation that @c std::length_error is thrown if appending
     * the character to the input sequence would require the condition
     * <tt>size() > max_size()</tt> to be true.
     */
    int_type overflow(int_type c)
    {
        if (!traits_type::eq_int_type(c, traits_type::eof()))
        {
            if (pptr() == epptr())
            {
                std::size_t _buffersize = pptr() - gptr();
                if (_buffersize < _max_size && _max_size - _buffersize < _bufferdelta)
                {
                    reserve(_max_size - _buffersize);
                }
                else
                {
                    reserve(_bufferdelta);
                }
            }

            *pptr() = traits_type::to_char_type(c);
            pbump(1);
            return c;
        }

        return traits_type::not_eof(c);
    }

    void reserve(std::size_t n)
    {
        // Get current stream positions as offsets.
        std::size_t gnext = gptr() - &_buffer[0];
        std::size_t pnext = pptr() - &_buffer[0];
        std::size_t pend = epptr() - &_buffer[0];

        // Check if there is already enough space in the put area.
        if (n <= pend - pnext)
        {
            return;
        }

        // Shift existing contents of get area to start of buffer.
        if (gnext > 0)
        {
            pnext -= gnext;
            std::memmove(&_buffer[0], &_buffer[0] + gnext, pnext);
        }

        // Ensure buffer is large enough to hold at least the specified size.
        if (n > pend - pnext)
        {
            if (n <= _max_size && pnext <= _max_size - n)
            {
                pend = pnext + n;
                _buffer.resize((std::max<std::size_t>)(pend, 1));
            }
            else
            {
                throw std::length_error("streambuf too long");
            }
        }

        // Update stream positions.
        setg(&_buffer[0], &_buffer[0], &_buffer[0] + pnext);
        setp(&_buffer[0] + pnext, &_buffer[0] + pend);
    }

private:
    std::size_t _max_size;
    std::vector<char_type, Allocator> _buffer;

    // Helper function to get the preferred size for reading data.
    friend std::size_t read_size_helper(basic_streambuf& sb, std::size_t max_size)
    {
        return std::min<std::size_t>(
            std::max<std::size_t>(512, sb._buffer.capacity() - sb.size()),
            std::min<std::size_t>(max_size, sb.max_size() - sb.size()));
    }
};

// Helper function to get the preferred size for reading data. Used for any
// user-provided specialisations of basic_streambuf.
template <typename Allocator>
inline std::size_t read_size_helper(basic_streambuf<Allocator>& sb, std::size_t max_size)
{
    return std::min<std::size_t>(512,
        std::min<std::size_t>(max_size, sb.max_size() - sb.size()));
}

/// Typedef for the typical usage of basic_streambuf.
typedef basic_streambuf<> streambuf;

NS_FK_END

#endif // FOUNDATIONKIT_BASIC_STREAMBUF_HPP
