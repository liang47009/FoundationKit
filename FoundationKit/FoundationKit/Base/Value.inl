/****************************************************************************
  Copyright (c) 2015 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
//================= Template Method =================

#include <cassert>

#ifndef LOSEMYMIND_VALUE_INL
#define LOSEMYMIND_VALUE_INL

NS_FK_BEGIN

template< typename T >
T Value::as()
{
    assert(false);
}


template<>
inline unsigned char Value::as<unsigned char>()
{
    assert(_type == Type::UCHAR);
    return _field._ucharVal;
}

template<>
inline unsigned short Value::as<unsigned short>()
{
    assert(_type == Type::USHORT);
    return _field._ushortVal;
}

template<>
inline unsigned int Value::as<unsigned int>()
{
    assert(_type == Type::UINT);
    return _field._uintVal;
}

template<>
inline unsigned long Value::as<unsigned long>()
{
    assert(_type == Type::ULONG);
    return _field._ulongVal;
}

template<>
inline unsigned long long Value::as<unsigned long long>()
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
inline short Value::as<short>()
{
    assert(_type == Type::SHORT);
    return _field._shortVal;
}

template<>
inline int Value::as<int>()
{
    assert(_type == Type::INT);
    return _field._intVal;
}

template<>
inline long Value::as<long>()
{
    assert(_type == Type::LONG);
    return _field._longVal;
}

template<>
inline long long Value::as<long long>()
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
#endif // LOSEMYMIND_VALUE_INL



