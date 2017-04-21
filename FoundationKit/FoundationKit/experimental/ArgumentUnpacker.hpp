/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_ARGUMENTUNPACKER_HPP
#define FOUNDATIONKIT_ARGUMENTUNPACKER_HPP

#include <vector>
#include "FoundationKit/GenericPlatformMacros.hpp"

NS_FK_BEGIN

// TEMPLATE CLASS ArgumentUnpackerImpl
template<bool _NotPointer>
struct ArgumentUnpackerImpl
{	
    template<typename _Ty>
    static char* unpack(_Ty& val, char* buffer)
    {
        size_t valSize;
        memcpy(&valSize, buffer, sizeof(valSize));
        buffer += sizeof(valSize);
        memcpy(&val, buffer, valSize);
        buffer += valSize;
        buffer += 1; // '\0' size
        return buffer;
    }

    template<>
    static char* unpack<std::string>(std::string& val, char* buffer)
    {
        size_t valSize;
        memcpy(&valSize, buffer, sizeof(valSize));
        buffer += sizeof(valSize);
        char* tmpVal = new char[valSize + 1];
        memset(tmpVal, 0, valSize + 1);
        memcpy(tmpVal, buffer, valSize);
        val = tmpVal;
        delete tmpVal;
        buffer += valSize;
        buffer += 1; // '\0' size
        return buffer;
    }
};

template<>
struct ArgumentUnpackerImpl <true> //is pointer
{
    template<typename _Ty>
    static char* unpack(_Ty& val, char* buffer)
    {
        size_t valSize;
        memcpy(&valSize, buffer, sizeof(valSize));
        buffer += sizeof(valSize);
        val = (_Ty)(buffer);
        buffer += valSize;
        buffer += 1; // '\0' size
        return buffer;
    }
};

template<typename _Ty>
static char* unpack(_Ty& val, char* buffer)
{
    return ArgumentUnpackerImpl<std::is_pointer<_Ty>::value>::unpack(val, buffer);
}

template<int N>
struct ArgumentUnpacker
{

    template<typename Tuple>
    static char* unpack(Tuple& tp, char* buffer)
    {
        return ::unpack(std::get<N - 1>(tp), ArgumentUnpacker<N - 1>::unpack(tp, buffer));
    }
};

template<>
struct ArgumentUnpacker <0>
{
    template<typename Tuple>
    static char* unpack(Tuple& tp, char* buffer)
    {
        UNUSED_ARG(tp);
        UNUSED_ARG(buffer);
        return buffer;// ::unpack(std::get<0>(tp), buffer);
    }
};

class ArgumentPacker
{
    std::vector<char> argument_data;
public:
    ArgumentPacker()
        : argument_data()
    {
    }
    template<typename _Ty>
    void pack_arg(const _Ty& val)
    {
        pack_arg(&val, sizeof(val));
    }

    void pack_arg(const void* data, size_t size)
    {
        char* sizePtr = (char*)&size;
        char* ptr = (char*)data;
        argument_data.insert(argument_data.end(), sizePtr, sizePtr + sizeof(size));
        argument_data.insert(argument_data.end(), ptr, ptr+size);
        argument_data.insert(argument_data.end(), 1, '\0');
    }

    template<typename Tuple>
    void unpack(Tuple& tp, const char* buffer)const
    {
        char* pData = (char*)buffer;
        ArgumentUnpacker<std::tuple_size<Tuple>::value>::unpack(tp, pData);
    }

    template<typename Tuple>
    void unpack(Tuple& tp)const
    {
        unpack(tp, &argument_data[0]);
    }
};

NS_FK_END

#endif // FOUNDATIONKIT_ARGUMENTUNPACKER_HPP
