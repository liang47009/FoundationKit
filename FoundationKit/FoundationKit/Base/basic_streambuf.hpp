
#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <streambuf>
#include <vector>
#include "FoundationKit/Base/mutable_data.hpp"

NS_FK_BEGIN

template <typename Allocator = std::allocator<char> >
class basic_streambuf : public std::streambuf
{
public:
    /// Construct a basic_streambuf object.
    /**
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

    /// Get the size of the input sequence.
    /**
    * @returns The size of the input sequence. The value is equal to that
    * calculated for @c s in the following code:
    * @code
    * size_t s = 0;
    * const_buffers_type bufs = data();
    * const_buffers_type::const_iterator i = bufs.begin();
    * while (i != bufs.end())
    * {
    *   const_buffer buf(*i++);
    *   s += _buffersize(buf);
    * }
    * @endcode
    */
    std::size_t size() const
    {
        return pptr() - gptr();
    }

    /// Get the maximum size of the basic_streambuf.
    /**
    * @returns The allowed maximum of the sum of the sizes of the input sequence
    * and output sequence.
    */
    std::size_t max_size() const
    {
        return _max_size;
    }

    /// Get a list of buffers that represents the input sequence.
    /**
    * @returns An object of type @c const_buffers_type that satisfies
    * ConstBufferSequence requirements, representing all character arrays in the
    * input sequence.
    *
    * @note The returned object is invalidated by any @c basic_streambuf member
    * function that modifies the input sequence or output sequence.
    */
    mutable_data data() const
    {
        return mutable_data(gptr(), (pptr() - gptr()) * sizeof(char_type));
    }

    /// Get a list of buffers that represents the output sequence, with the given
    /// size.
    /**
    * Ensures that the output sequence can accommodate @c n characters,
    * reallocating character array objects as necessary.
    *
    * @returns An object of type @c mutable_data that satisfies
    * MutableBufferSequence requirements, representing character array objects
    * at the start of the output sequence such that the sum of the buffer sizes
    * is @c n.
    *
    * @throws std::length_error If <tt>size() + n > max_size()</tt>.
    *
    * @note The returned object is invalidated by any @c basic_streambuf member
    * function that modifies the input sequence or output sequence.
    */
    mutable_data prepare(std::size_t n)
    {
        reserve(n);
        return mutable_data(pptr(), n * sizeof(char_type));
    }

    /// Move characters from the output sequence to the input sequence.
    /**
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

    /// Remove characters from the input sequence.
    /**
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

protected:
    enum { _bufferdelta = 128 };

    /// Override std::streambuf behaviour.
    /**
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

    /// Override std::streambuf behaviour.
    /**
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
                std::length_error ex("asio::streambuf too long");
                asio::detail::throw_exception(ex);
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
    friend std::size_t read_size_helper(
        basic_streambuf& sb, std::size_t max_size)
    {
        return std::min<std::size_t>(
            std::max<std::size_t>(512, sb._buffer.capacity() - sb.size()),
            std::min<std::size_t>(max_size, sb.max_size() - sb.size()));
    }
};





NS_FK_END

