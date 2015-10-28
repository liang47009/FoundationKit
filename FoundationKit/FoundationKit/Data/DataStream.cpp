/****************************************************************************
  Copyright (c) 2013-2014 libo.
 
  losemymind.libo@gmail.com

****************************************************************************/

#include <sstream>
#include "DataStream.h"

NS_FK_BEGIN

DataStream::DataStream()
{
}

DataStream::DataStream(DataStream&& pDataStream): 
		_buffer(std::move(pDataStream._buffer))
{
}

DataStream& DataStream::operator=(DataStream&& pDataStream)
{
	_buffer = std::move(pDataStream._buffer);
	return *this;
}

DataStream& DataStream::operator<<(const unsigned char data)
{
    _buffer.append((char*)&data, sizeof(unsigned char));
    return *this;
}

DataStream& DataStream::operator<<(const unsigned short data)
{
    _buffer.append((char*)&data, sizeof(unsigned short));
    return *this;
}

DataStream& DataStream::operator<<(const unsigned int data)
{
    _buffer.append((char*)&data, sizeof(unsigned int));
    return *this;
}

DataStream& DataStream::operator<<(const unsigned long data)
{
    _buffer.append((char*)&data, sizeof(unsigned long));
    return *this;
}

DataStream& DataStream::operator<<(const unsigned long long data)
{
    _buffer.append((char*)&data, sizeof(unsigned long long));
    return *this;
}

DataStream& DataStream::operator<<(bool data)
{
	*this << (data ? 1 : 0);
	return *this;
}

DataStream& DataStream::operator<<(char data)
{
	_buffer.append((char*)&data, sizeof(char));
	return *this;
}

DataStream& DataStream::operator<<(const short data)
{
	_buffer.append((char*)&data, sizeof(short));
	return *this;
}

DataStream& DataStream::operator<<(const int data)
{
	_buffer.append((char*)&data, sizeof(int));
	return *this;
}

DataStream& DataStream::operator<<(const long data)
{
    _buffer.append((char*)&data, sizeof(long));
    return *this;
}

DataStream& DataStream::operator<<(const long long data)
{
	_buffer.append((char*)&data, sizeof(long long));
	return *this;
}

DataStream& DataStream::operator<<(const float data)
{
	_buffer.append((char*)&data, sizeof(float));
	return *this;
}

DataStream& DataStream::operator<<(const double data)
{
	_buffer.append((char*)&data, sizeof(double));
	return *this;
}

DataStream& DataStream::operator<<( const char* data )
{
	*this << std::string(data);
	return *this;
}

DataStream& DataStream::operator<<(const std::string& data)
{
	*this << data.size();
	_buffer.append(data.c_str(), data.size());
	return *this;
}

DataStream& DataStream::operator>>(unsigned char& data)
{
    read(data);
    return *this;
}

DataStream& DataStream::operator>>(unsigned short& data)
{
    read(data);
    return *this;
}

DataStream& DataStream::operator>>(unsigned int& data)
{
    read(data);
    return *this;
}

DataStream& DataStream::operator>>(unsigned long& data)
{
    read(data);
    return *this;
}

DataStream& DataStream::operator>>(unsigned long long& data)
{
    read(data);
    return *this;
}

DataStream& DataStream::operator>>(bool& data)
{
	unsigned char i;
	*this >> i;
	data = i ? true : false;
	return *this;
}

DataStream& DataStream::operator>>(char& data)
{
	read(data);
	return *this;
}


DataStream& DataStream::operator>>(short& data)
{
	read(data);
	return *this;
}

DataStream& DataStream::operator>>(int& data)
{
	read(data);
	return *this;
}

DataStream& DataStream::operator>>(long& data)
{
    read(data);
    return *this;
}

DataStream& DataStream::operator>>(long long& data)
{
	read(data);
	return *this;
}

DataStream& DataStream::operator>>(float& data)
{
	read(data);
	return *this;
}

DataStream& DataStream::operator>>(double& data)
{
	read(data);
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

	data = _buffer.substr(0,size);
	data.shrink_to_fit();
	_buffer.erase(0,size);
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
}

void DataStream::read( unsigned char* data, size_t dataSize )
{
	memcpy(&data,&_buffer[0],dataSize);
	_buffer.erase(0,dataSize);
}

void DataStream::clear()
{
	_buffer.clear();
}

void DataStream::reset(const std::string& data)
{
	_buffer.clear();
	_buffer.append(&data[0], data.size());
}

size_t DataStream::size()
{
	return (int)_buffer.size();
}

const std::string& DataStream::getBuffer()
{
	return _buffer;
}

const char* DataStream::c_str()
{
	return _buffer.c_str();
}

NS_FK_END