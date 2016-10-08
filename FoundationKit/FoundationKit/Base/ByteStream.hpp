

#include <vector>
#include <cassert>
#include "FoundationKit/FoundationMacros.h"
#include "FoundationKit/Base/Types.h"

NS_FK_BEGIN


///
/// @code
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
/// @endcode
class ByteStream
{
public:
    typedef unsigned char  _Elem;
    ByteStream()
        : _readIndex(0)
        , _writeIndex(0)
    {

    }

    ByteStream(const ByteStream& other)
    {
        copy(other);
    }

    ByteStream(ByteStream&& other)
    {
        move(std::forward<ByteStream&&>(other));
    }

    ByteStream& operator=(ByteStream&& other)
    {
        if (this != &other)
        {
            move(std::forward<ByteStream&&>(other));
        }
        return *this;
    }

    ByteStream& operator=(const ByteStream& other)
    {
        if (this != &other)
        {
            copy(other);
        }
        return *this;
    }

    // Compare operator.
    bool operator ==(const ByteStream& other) const
    {
        if (this != &other)
        {
            if (_writeIndex == other._writeIndex)
            {
                if (std::memcmp(this->data(), other.data(), _writeIndex * sizeof(_Elem)) == 0)
                {
                    return true;
                }
            }
            return false;
        }
        return true;
    }

    // Compare operator.
    bool operator !=(const ByteStream& other) const
    {
        return !(*this == other);
    }

    _Elem* data()
    {
        return _buffer.data();
    }

    const _Elem* data()const
    {
        return _buffer.data();
    }

    size_t size()
    {
        return _buffer.size();
    }

    _Elem* begin()
    {
        return &_buffer[0];
    }

    const _Elem* begin() const
    {
        return &_buffer[0];
    }

    _Elem* end()
    {
        return &(*_buffer.end());
    }


    const _Elem* end() const
    {
        return &(*_buffer.end());
    }

    bool empty() const
    {
        return _buffer.empty();
    }

    void expansion(size_t appandSize)
    {
        size_t newSize = _buffer.size() + appandSize;
        _buffer.reserve(newSize);
    }
    void swap(ByteStream& other)
    {
        this->_buffer.swap(other._buffer);
        std::swap(this->_readIndex, other._readIndex);
        std::swap(this->_writeIndex, other._writeIndex);
    }

    void clear()
    {
        _buffer.clear();
        this->_readIndex = 0;
        this->_writeIndex = 0;
    }

protected:
    void copy(const ByteStream& other)
    {
        this->_buffer = other._buffer;
        this->_readIndex = other._readIndex;
        this->_writeIndex = other._writeIndex;
    }

    void move(ByteStream&& other)
    {
        this->_buffer = std::move(other._buffer);
        this->_readIndex = other._readIndex;
        this->_writeIndex = other._writeIndex;
        other.clear();
    }

private:
    std::vector<_Elem> _buffer;
    size_t             _readIndex;
    size_t             _writeIndex;
};


NS_FK_END





