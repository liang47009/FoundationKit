/****************************************************************************
  Copyright (c) 2015 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/

#include <sstream>
#include "DataStream.h"

NS_FK_BEGIN

DataStream::DataStream()
: _readIndex(0)
, _burnAfterReading(false)
{
}
DataStream::DataStream(const DataStream& pDataStream)
	: _buffer(pDataStream.getBuffer())
    , _readIndex(pDataStream._readIndex)
    , _burnAfterReading(pDataStream._burnAfterReading)
{
}

DataStream::DataStream(DataStream&& pDataStream)
	: _buffer(std::move(pDataStream._buffer))
    , _readIndex(pDataStream._readIndex)
    , _burnAfterReading(pDataStream._burnAfterReading)
{
}

DataStream& DataStream::operator=(const DataStream& pDataStream)
{
	_buffer = pDataStream.getBuffer();
    _burnAfterReading = pDataStream._burnAfterReading;
    _readIndex = pDataStream._readIndex;
	return *this;
}

DataStream& DataStream::operator=(DataStream&& pDataStream)
{
	_buffer = std::move(pDataStream._buffer);
    _burnAfterReading = pDataStream._burnAfterReading;
    _readIndex = pDataStream._readIndex;
	return *this;
}

DataStream& DataStream::operator << (const char* data)
{
    *this << std::string(data);
    return *this;
}

DataStream& DataStream::operator << (const std::string& data)
{
    *this << data.size();
    _buffer.append(data.c_str(), data.size());
    return *this;
}

DataStream& DataStream::operator >> (char* data)
{
    unsigned int size = 0;
    *this >> size;

    // Check for fake string size to prevent memory hacks
    if (size > _buffer.size())
    {
        std::ostringstream os;
        os << "String size (" << size << ") > packet size (" << _buffer.size() << ")";
        throw std::out_of_range(os.str());
    }
    if (size == 0)
    {
        data = (char*)"";
        return *this;
    }
    read((unsigned char*)data, size);
    return *this;
}

DataStream& DataStream::operator>>(std::string& data)
{
	unsigned int size = 0;
	*this >> size;

	// Check for fake string size to prevent memory hacks
	if(size > _buffer.size())
	{
		std::ostringstream os;
		os << "String size (" << size << ") > packet size (" << _buffer.size() << ")";
		throw std::out_of_range(os.str());
	}
	if(size == 0)
	{
		data = "";
		return *this;
	}

	//data = _buffer.substr(0,size);
    data = _buffer.substr(getReadIndex(), size);
	data.shrink_to_fit();
	//_buffer.erase(0,size);
    readIndexIncrement(size);
	return *this;
}

void DataStream::write(unsigned char* data, size_t pSize, int pPos)
{
	if(pPos < 0)
	{
		_buffer.append(reinterpret_cast<char*>(data), pSize);
	}
	else if(pPos + pSize <= _buffer.size())
	{
		memcpy(&_buffer[pPos], data, pSize);
	}
	else
	{
		std::ostringstream os;
		os << "pPos + pSize > _buffer.size()";
		throw std::out_of_range(os.str());
	}
}

void DataStream::read( unsigned char* data, size_t dataSize )
{
	//memcpy(data,&_buffer[0],dataSize);
	//_buffer.erase(0,dataSize);
    memcpy(data, &_buffer[getReadIndex()], dataSize);
    readIndexIncrement(dataSize);
}

void DataStream::clear()
{
	_buffer.clear();
    _readIndex = 0;
}

void DataStream::reset(const std::string& data)
{
	_buffer.clear();
	_buffer.append(&data[0], data.size());
    _readIndex = 0;
}

size_t DataStream::size()
{
	return _buffer.size();
}

const std::string& DataStream::getBuffer()const
{
	return _buffer;
}

const char* DataStream::c_str()
{
	return _buffer.c_str();
}

DataStream::size_type DataStream::getReadIndex()
{
    if (_burnAfterReading)
       return 0;
    return _readIndex;
}

void DataStream::readIndexIncrement(size_type count)
{
    if (_burnAfterReading)
        _buffer.erase(0, count);
    else
        _readIndex += count;
}

NS_FK_END