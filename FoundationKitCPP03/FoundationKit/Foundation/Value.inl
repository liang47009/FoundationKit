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

template< typename T >
T Value::as()
{
    assert(false);
}

template<>
inline uint8 Value::as<uint8>()
{
    assert(_type == Type::UCHAR);
    return _field._ucharVal;
}

template<>
inline uint16 Value::as<uint16>()
{
    assert(_type == Type::USHORT);
    return _field._ushortVal;
}

template<>
inline uint32 Value::as<uint32>()
{
    assert(_type == Type::UINT);
    return _field._uintVal;
}

template<>
inline uint64 Value::as<uint64>()
{
    assert(_type == Type::ULONGLONG);
    return _field._ulonglongVal;
}

template<>
inline bool Value::as<bool>()
{
    assert(_type == Type::BOOL);
    return _field._boolVal;
}

template<>
inline char Value::as<char>()
{
    assert(_type == Type::CHAR);
    return _field._charVal;
}

template<>
inline short Value::as<int16>()
{
    assert(_type == Type::SHORT);
    return _field._shortVal;
}

template<>
inline int Value::as<int32>()
{
    assert(_type == Type::INT);
    return _field._intVal;
}

template<>
inline long long Value::as<int64>()
{
    assert(_type == Type::LONGLONG);
    return _field._longlongVal;
}

template<>
inline float Value::as<float>()
{
    assert(_type == Type::FLOAT);
    return _field._floatVal;
}

template<>
inline double Value::as<double>()
{
    assert(_type == Type::DOUBLE);
    return _field._doubleVal;
}

template<>
inline char* Value::as<char*>()
{
    assert(_type == Type::PCHAR);
    return _field._pcharVal;
}

template<>
inline std::string Value::as<std::string>()
{
    assert(_type == Type::STRING);
    return _field._stringVal;
}

NS_FK_END

#endif // FOUNDATIONKIT_VALUE_INL