/****************************************************************************
  Copyright (c) 2015 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef LOSEMYMIND_DATASTREAM_H
#define LOSEMYMIND_DATASTREAM_H

#pragma once

#include <cstdint>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <string>
#include "FoundationKit/GenericPlatformMacros.h"
NS_FK_BEGIN

class  DataStream
{
public:
    typedef std::string::size_type size_type;
	DataStream();
	DataStream(const DataStream& pDataStream);
	DataStream(DataStream&& pDataStream);
	DataStream& operator=(const DataStream& pDataStream);
	DataStream& operator=(DataStream&& pDataStream);

    template<typename T, typename = typename std::enable_if<std::is_fundamental<T>::value>::type >
    DataStream& operator<<(T data)
    {
        _buffer.append((char*)&data, sizeof(T));
        return *this;
    }

    DataStream& operator<<(const char* data);
    DataStream& operator<<(const std::string& data);

    template<typename T, typename = typename std::enable_if<std::is_fundamental<T>::value>::type >
    DataStream& operator>>(T& data)
    {
        read(data);
        return *this;
    }

    DataStream& operator>>(char* data);
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

	void write(uint8_t* data, size_t pSize, int32_t pPos = -1);

	template< typename T >
	void read(T& data)
	{
		if (size() < sizeof(T))
		{
			data = T();
			return;
		}
        memcpy(&data, &_buffer[getReadIndex()], sizeof(T));
        readIndexIncrement(sizeof(T));
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
	const std::string&	getBuffer()const;

    void  setBurnAfterReading(bool val){ _burnAfterReading = val; }
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

    size_type getReadIndex();

    void readIndexIncrement(size_type count);

protected:
	std::string _buffer;
    size_type   _readIndex;
    bool        _burnAfterReading;
};

NS_FK_END
#endif // LOSEMYMIND_DATASTREAM_H





