/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.

  losemymind.libo@gmail.com

****************************************************************************/

#include <sstream>
#include <assert.h>
#include "Value.hpp"

NS_FK_BEGIN

const Value Value::Null;

Value::Value(Type valType)
    : _type(valType)
{
    memset(&_field, 0x00, sizeof(_field));
}


Value::Value()
    : Value(Type::NONE)
{
    
}

Value::~Value()
{
    this->clear();
}

Value::Value(const Value& other)
    : Value()
{
    copy(const_cast<Value&>(other));
}


Value::Value(Value&& other)
    : Value()
{
    move(std::forward<Value>(other));
}

Value::Value(unsigned char data)
    : Value(Type::UCHAR)
{
    _field._ucharVal = data;
}

Value::Value(unsigned short data)
    : Value(Type::USHORT)
{
    _field._ushortVal = data;
}

Value::Value(unsigned int data)
    : Value(Type::UINT)
{
    _field._uintVal = data;
}

Value::Value(unsigned long long data)
    : Value(Type::ULONGLONG)
{
    _field._ulonglongVal = data;
}

Value::Value(bool data)
    : Value(Type::BOOL)
{
    _field._boolVal = data;
}

Value::Value(char data)
    : Value(Type::CHAR)
{
    _field._charVal = data;
}

Value::Value(short data)
    : Value(Type::SHORT)
{
    _field._shortVal = data;
}

Value::Value(int data)
    : Value(Type::INT)
{
    _field._intVal = data;
}

Value::Value(long long data)
    : Value(Type::LONGLONG)
{
    _field._longlongVal = data;
}

Value::Value(float data)
    : Value(Type::FLOAT)
{
    _field._floatVal = data;
}

Value::Value(double data)
    : Value(Type::DOUBLE)
{
    _field._doubleVal = data;
}

Value::Value(const char* data)
    : Value(Type::PCHAR)
{
    size_t len = strlen(data);
    _field._pcharVal = new char[len + 1];
    memcpy(_field._pcharVal, data, len);
    _field._pcharVal[len] = '\0';
}

Value::Value(const std::string& data)
    : Value(Type::STRING)
{
    _field._stringVal = new char[data.size() + 1];
    memcpy(_field._stringVal, data.c_str(), data.size());
    _field._stringVal[data.size()] = '\0';
}

Value& Value::operator=(const Value& value)
{
    copy(const_cast<Value&>(value));
    return *this;
}

Value& Value::operator=(Value&& other)
{
    move(std::forward<Value>(other));
    return *this;
}
Value& Value::operator=(unsigned char data)
{
    reset(Type::UCHAR);
    _field._ucharVal = data;
    return *this;
}
Value& Value::operator=(unsigned short data)
{
    reset(Type::USHORT);
    _field._ushortVal = data;
    return *this;
}
Value& Value::operator=(unsigned int data)
{
    reset(Type::UINT);
    _field._uintVal = data;
    return *this;
}


Value& Value::operator=(unsigned long long data)
{
    reset(Type::ULONGLONG);
    _field._ulonglongVal = data;
    return *this;
}
Value& Value::operator=(bool data)
{
    reset(Type::BOOL);
    _field._boolVal = data;
    return *this;
}
Value& Value::operator=(char data)
{
    reset(Type::CHAR);
    _field._charVal = data;
    return *this;
}
Value& Value::operator=(short data)
{
    reset(Type::SHORT);
    _field._shortVal = data;
    return *this;
}
Value& Value::operator=(int data)
{
    reset(Type::INT);
    _field._intVal = data;
    return *this;
}

Value& Value::operator=(long long data)
{
    reset(Type::LONGLONG);
    _field._longlongVal = data;
    return *this;
}
Value& Value::operator=(float data)
{
    reset(Type::FLOAT);
    _field._floatVal = data;
    return *this;
}
Value& Value::operator=(double data)
{
    reset(Type::DOUBLE);
    _field._doubleVal = data;
    return *this;
}
Value& Value::operator=(const char* data)
{
    reset(Type::PCHAR);
    size_t len = strlen(data);
    _field._pcharVal = new char[len + 1];
    memcpy(_field._pcharVal, data, len);
    _field._pcharVal[len] = '\0';
    return *this;
}
Value& Value::operator=(const std::string& data)
{
    reset(Type::STRING);
    _field._stringVal = new char[data.size() + 1];
    memcpy(_field._stringVal, data.c_str(), data.size());
    _field._stringVal[data.size()] = '\0';
    return *this;
}

bool Value::operator!= (const Value& other)
{
    return !(*this == other);
}

bool Value::operator!= (const Value& other) const
{
    return !(*this == other);
}

bool Value::operator== (const Value& other)
{
    const auto &t = *this;
    return t == other;
}

bool Value::operator== (const Value& other) const
{
    if (this == &other) return true;
    if (other._type != this->_type) return false;
    if (this->isNull()) return true;
    switch (_type)
    {
    case Value::Type::NONE:
        break;
    case Value::Type::UCHAR: return _field._ucharVal   == other._field._ucharVal;
    case Value::Type::USHORT: return _field._ushortVal == other._field._ushortVal;
    case Value::Type::UINT: return _field._uintVal == other._field._uintVal;
    case Value::Type::ULONGLONG: return _field._ulonglongVal == other._field._ulonglongVal;
    case Value::Type::BOOL: return _field._boolVal == other._field._boolVal;
    case Value::Type::CHAR: return _field._charVal == other._field._charVal;
    case Value::Type::SHORT: return _field._shortVal == other._field._shortVal;
    case Value::Type::INT: return _field._intVal == other._field._intVal;
    case Value::Type::LONGLONG: return _field._longlongVal == other._field._longlongVal;
    case Value::Type::FLOAT: return _field._floatVal == other._field._floatVal;
    case Value::Type::DOUBLE: return _field._doubleVal == other._field._doubleVal;
    case Value::Type::PCHAR: return strcmp(_field._pcharVal, other._field._pcharVal) == 0;
    case Value::Type::STRING: return strcmp(_field._stringVal, other._field._stringVal) == 0;
    default:
        break;
    }
    return false;
}

void Value::copy(Value& other)
{
    _type = other.getType();
    switch (_type)
    {
    case Value::Type::NONE:
        memset(&_field, 0, sizeof(_field));
        break;
    case Value::Type::UCHAR:
        _field._ucharVal = other.as<unsigned char>();
        break;
    case Value::Type::USHORT:
        _field._ushortVal = other.as<unsigned short>();
        break;
    case Value::Type::UINT:
        _field._uintVal = other.as<unsigned int>();
        break;
    case Value::Type::ULONGLONG:
        _field._ulonglongVal = other.as<unsigned long long>();
        break;
    case Value::Type::BOOL:
        _field._boolVal = other.as<bool>();
        break;
    case Value::Type::CHAR:
        _field._charVal = other.as<char>();
        break;
    case Value::Type::SHORT:
        _field._shortVal = other.as<short>();
        break;
    case Value::Type::INT:
        _field._intVal = other.as<int>();
        break;
    case Value::Type::LONGLONG:
        _field._longlongVal = other.as<long long>();
        break;
    case Value::Type::FLOAT:
        _field._floatVal = other.as<float>();
        break;
    case Value::Type::DOUBLE:
        _field._doubleVal = other.as<double>();
        break;
    case Value::Type::PCHAR:
        {
            SAFE_DELETE(_field._pcharVal);
            char* val = other.as<char*>();
            size_t len = strlen(val);
            _field._pcharVal = new char[len + 1];
            memcpy(_field._pcharVal, val, len);
            _field._pcharVal[len] = '\0';
        }
        break;
    case Value::Type::STRING:
        {
            SAFE_DELETE(_field._stringVal);
            std::string val = other.as<std::string>();
            _field._stringVal = new char[val.size() + 1];
            memcpy(_field._stringVal, val.c_str(), val.size());
            _field._stringVal[val.size()] = '\0';
        }
        break;
    default:
        break;
    }
}

void Value::move(Value&& other)
{
    _type = std::move(other._type);
    _field = std::move(other._field);
    memset(&other._field, 0, sizeof(other._field));
    other._type = Type::NONE; 
}

void Value::swap(Value& other)
{
    std::swap(_type, other._type);
    std::swap(_field, other._field);
}

void Value::clear()
{
    if (_type == Type::NONE)
        return;
    
    switch (_type)
    {
    case Value::Type::PCHAR:
        SAFE_DELETE_ARRAY(_field._pcharVal);
        break;
    case Value::Type::STRING:
        SAFE_DELETE_ARRAY(_field._stringVal);
        break;
    default:
        break;
    }
    memset(&_field, 0, sizeof(_field));
    _type = Type::NONE;
}


void Value::reset(Type valType)
{
    if (_type == valType)
        return;
    if (_type == Type::PCHAR)
    {
        SAFE_DELETE_ARRAY(_field._pcharVal);
    }
    else if (_type == Type::STRING)
    {
        SAFE_DELETE_ARRAY(_field._stringVal);
    }
    _type = valType;
}

NS_FK_END




