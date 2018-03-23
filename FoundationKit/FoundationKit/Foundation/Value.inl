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
Value::Value(_Ty data)
    : Value(EType::OTHER)
{
    _field._otherData = malloc(sizeof(_Ty));
    memcpy(_field._otherData, &data, sizeof(_Ty));
}

template<typename _Ty>
Value&  Value::operator= (_Ty data)
{
    Reset(EType::OTHER);
    _field._otherData = malloc(sizeof(_Ty));
    memcpy(_field._otherData, &data, sizeof(_Ty));
    return *this;
}

template< typename _Ty>
_Ty Value::As()
{
    assert(_type == EType::OTHER);
    _Ty obj;
    memcpy(&obj, _field._otherData, sizeof(_Ty));
    return obj;
}

template<>
inline uint8 Value::As<uint8>()
{
    assert(_type == EType::UCHAR);
    return _field._ucharVal;
}

template<>
inline uint16 Value::As<uint16>()
{
    assert(_type == EType::USHORT);
    return _field._ushortVal;
}

template<>
inline uint32 Value::As<uint32>()
{
    assert(_type == EType::UINT);
    return _field._uintVal;
}

template<>
inline uint64 Value::As<uint64>()
{
    assert(_type == EType::ULONGLONG);
    return _field._ulonglongVal;
}

template<>
inline bool Value::As<bool>()
{
    assert(_type == EType::BOOL);
    return _field._boolVal;
}

template<>
inline char Value::As<char>()
{
    assert(_type == EType::CHAR);
    return _field._charVal;
}

template<>
inline short Value::As<int16>()
{
    assert(_type == EType::SHORT);
    return _field._shortVal;
}

template<>
inline int Value::As<int32>()
{
    assert(_type == EType::INT);
    return _field._intVal;
}

template<>
inline long long Value::As<int64>()
{
    assert(_type == EType::LONGLONG);
    return _field._longlongVal;
}

template<>
inline float Value::As<float>()
{
    assert(_type == EType::FLOAT);
    return _field._floatVal;
}

template<>
inline double Value::As<double>()
{
    assert(_type == EType::DOUBLE);
    return _field._doubleVal;
}

template<>
inline char* Value::As<char*>()
{
    assert(_type == EType::STRING);
    return _field._stringVal;
}

template<>
inline const char* Value::As<const char*>()
{
    assert(_type == EType::STRING);
    return _field._stringVal;
}

template<>
inline std::string Value::As<std::string>()
{
    assert( _type == EType::STRING);
    return _field._stringVal;
}

NS_FK_END

#endif // FOUNDATIONKIT_VALUE_INL

