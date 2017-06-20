/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_VALUE_INL
#define FOUNDATIONKIT_VALUE_INL

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cassert>

NS_FK_BEGIN

template<typename _Ty>
Value::Value(_Ty* data)
    : Value(Type::POINTER)
{
    _field._pointer = data;
}

template<typename _Ty>
Value&  Value::operator= (_Ty* data)
{
    Reset(Type::POINTER);
    _field._pointer = data;
    return *this;
}

template< typename T >
T Value::As()
{
    if (_type == Type::POINTER && std::is_pointer<T>::value)
        return static_cast<T>(_field._pointer);
    assert(false);
    return T();
}

template<>
inline uint8 Value::As<uint8>()
{
    assert(_type == Type::UCHAR);
    return _field._ucharVal;
}

template<>
inline uint16 Value::As<uint16>()
{
    assert(_type == Type::USHORT);
    return _field._ushortVal;
}

template<>
inline uint32 Value::As<uint32>()
{
    assert(_type == Type::UINT);
    return _field._uintVal;
}

template<>
inline uint64 Value::As<uint64>()
{
    assert(_type == Type::ULONGLONG);
    return _field._ulonglongVal;
}

template<>
inline bool Value::As<bool>()
{
    assert(_type == Type::BOOL);
    return _field._boolVal;
}

template<>
inline char Value::As<char>()
{
    assert(_type == Type::CHAR);
    return _field._charVal;
}

template<>
inline short Value::As<int16>()
{
    assert(_type == Type::SHORT);
    return _field._shortVal;
}

template<>
inline int Value::As<int32>()
{
    assert(_type == Type::INT);
    return _field._intVal;
}

template<>
inline long long Value::As<int64>()
{
    assert(_type == Type::LONGLONG);
    return _field._longlongVal;
}

template<>
inline float Value::As<float>()
{
    assert(_type == Type::FLOAT);
    return _field._floatVal;
}

template<>
inline double Value::As<double>()
{
    assert(_type == Type::DOUBLE);
    return _field._doubleVal;
}

template<>
inline char* Value::As<char*>()
{
    assert(_type == Type::PCHAR || _type == Type::STRING);
    if (_type == Type::PCHAR)
        return _field._pcharVal;
    else
        return _field._stringVal;
}

template<>
inline std::string Value::As<std::string>()
{
    assert(_type == Type::STRING || _type == Type::PCHAR);
    if (_type == Type::PCHAR)
        return _field._pcharVal;
    else
        return _field._stringVal;
}


NS_FK_END

#endif // FOUNDATIONKIT_VALUE_INL

