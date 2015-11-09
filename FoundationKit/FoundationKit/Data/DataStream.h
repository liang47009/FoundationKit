/****************************************************************************
  Copyright (c) 2013-2014 libo.
 
  losemymind.libo@gmail.com

****************************************************************************/
#pragma once

#include <cstdint>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <string>
#include "FoundationKit/FoundationKitDefines.h"
NS_FK_BEGIN
class  DataStream
{
public:
	DataStream();
	DataStream(DataStream&& pDataStream);
	DataStream& operator=(DataStream&& pDataStream);
    DataStream& operator<<(const unsigned char data);
    DataStream& operator<<(const unsigned short data);
    DataStream& operator<<(const unsigned int data);
    DataStream& operator<<(const unsigned long data);
    DataStream& operator<<(const unsigned long long data);
	DataStream& operator<<(bool data);
	DataStream& operator<<(const char data);
	DataStream& operator<<(const short data);
	DataStream& operator<<(const int data);
    DataStream& operator<<(const long data);
	DataStream& operator<<(const long long data);
	DataStream& operator<<(const float data);
	DataStream& operator<<(const double data);
	DataStream& operator<<(const char* data);
	DataStream& operator<<(const std::string& data);

    DataStream& operator>>(unsigned char& data);
    DataStream& operator>>(unsigned short& data);
    DataStream& operator>>(unsigned int& data);
    DataStream& operator>>(unsigned long& data);
    DataStream& operator>>(unsigned long long& data);
    DataStream& operator>>(bool& data);
	DataStream& operator>>(char& data);
	DataStream& operator>>(short& data);
	DataStream& operator>>(int& data);
    DataStream& operator>>(long& data);
	DataStream& operator>>(long long& data);
	DataStream& operator>>(float& data);
	DataStream& operator>>(double& data);
	DataStream& operator>>(std::string& data);

	template <typename K, typename V>
	DataStream& operator<<(const std::map<K, V>& data)
	{
		return writeAssociativeContainer(data);
	}

	template <typename K, typename V>
	DataStream& operator>>(std::map<K, V>& data)
	{
		return readAssociativeContainer<std::map<K, V>, K, V>(data);
	}

	template <typename K, typename V>
	DataStream& operator<<(const std::unordered_map<K, V>& data)
	{
		return writeAssociativeContainer(data);
	}

	template <typename K, typename V>
	DataStream& operator>>(std::unordered_map<K, V>& data)
	{
		return readAssociativeContainer<std::unordered_map<K, V>, K, V>(data);
	}

	template <typename V>
	DataStream& operator<<(const std::vector<V>& data)
	{
		return writeSequenceContainer(data);
	}

	template <typename V>
	DataStream& operator>>(std::vector<V>& data)
	{
		return readSequenceContainer<std::vector<V>, V>(data);
	}

	template <typename V>
	DataStream& operator<<(const std::list<V>& data)
	{
		return writeSequenceContainer(data);
	}

	template <typename V>
	DataStream& operator>>(std::list<V>& data)
	{
		return readSequenceContainer<std::list<V>, V>(data);
	}

	void write (uint8_t* data, size_t pSize, int32_t pPos = -1);

	template< typename T >
	void read(T& data)
	{
		if(_buffer.size() < sizeof(T))
		{
			data = 0;
			return;
		}
		memcpy(&data,&_buffer[0],sizeof(T));
		_buffer.erase(0,sizeof(T));
	}

	template< typename T >
	inline T read()
	{
		T ret;
		read(ret);
		return ret;
	}

	void read(uint8_t* data, size_t dataSize);


	void   clear();
	void   reset(const std::string& data);
	size_t size();

	const char* c_str();

	const std::string&	getBuffer();

private:
	template<typename C>
	DataStream& writeSequenceContainer(const C& data)
	{
		*this << data.size();
		for(auto iter : data)
		{
			*this << iter;
		}
		return *this;
	}

	template<typename C, typename V>
	DataStream& readSequenceContainer(C& data)
	{
		size_t size=0;
		*this >> size;

		for(size_t i = 0; i < size; ++i)
		{
			V value;
			*this >> value;
			data.push_back(value);
		}

		return *this;
	}

	template<typename C>
	DataStream& writeAssociativeContainer(const C& data)
	{
		*this << data.size();
		for(auto& iter : data)
		{
			*this << iter.first << iter.second;
		}
		return *this;
	}

	template<typename C ,typename K, typename V>
	DataStream& readAssociativeContainer(C& data)
	{
		size_t size=0;
		*this >> size;
		for(size_t i = 0; i < size; ++i)
		{
			K key;
			V value;
			*this >> key >> value;
			data.insert(std::pair<K,V>(key, value));
		}
		return *this;
	}

protected:
	std::string		                  _buffer;

};

NS_FK_END