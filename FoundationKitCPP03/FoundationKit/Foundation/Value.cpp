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
: _type(Type::NONE)
{
    memset(&_field, 0x00, sizeof(_field));
}


Value::~Value()
{
    this->Clear();
}

Value::Value(const Value& other)
{
    memset(&_field, 0x00, sizeof(_field));
    Copy(const_cast<Value&>(other));
}

Value::Value(unsigned char data)
    : _type(Type::UCHAR)
{
    memset(&_field, 0x00, sizeof(_field));
    _field._ucharVal = data;
}

Value::Value(unsigned short data)
    : _type(Type::USHORT)
{
    memset(&_field, 0x00, sizeof(_field));
    _field._ushortVal = data;
}

Value::Value(unsigned int data)
    : _type(Type::UINT)
{
    memset(&_field, 0x00, sizeof(_field));
    _field._uintVal = data;
}

Value::Value(unsigned long long data)
    : _type(Type::ULONGLONG)
{
    memset(&_field, 0x00, sizeof(_field));
    _field._ulonglongVal = data;
}

Value::Value(bool data)
    : _type(Type::BOOL)
{
    memset(&_field, 0x00, sizeof(_field));
    _field._boolVal = data;
}

Value::Value(char data)
    : _type(Type::CHAR)
{
    memset(&_field, 0x00, sizeof(_field));
    _field._charVal = data;
}

Value::Value(short data)
    : _type(Type::SHORT)
{
    memset(&_field, 0x00, sizeof(_field));
    _field._shortVal = data;
}

Value::Value(int data)
    : _type(Type::INT)
{
    memset(&_field, 0x00, sizeof(_field));
    _field._intVal = data;
}

Value::Value(long long data)
    : _type(Type::LONGLONG)
{
    memset(&_field, 0x00, sizeof(_field));
    _field._longlongVal = data;
}

Value::Value(float data)
    : _type(Type::FLOAT)
{
    memset(&_field, 0x00, sizeof(_field));
    _field._floatVal = data;
}

Value::Value(double data)
    : _type(Type::DOUBLE)
{
    memset(&_field, 0x00, sizeof(_field));
    _field._doubleVal = data;
}

Value::Value(const char* data)
    : _type(Type::PCHAR)
{
    memset(&_field, 0x00, sizeof(_field));
    size_t len = strlen(data);
    _field._pcharVal = new char[len + 1];
    memcpy(_field._pcharVal, data, len);
    _field._pcharVal[len] = '\0';
}

Value::Value(const std::string& data)
    : _type(Type::STRING)
{
    memset(&_field, 0x00, sizeof(_field));
    _field._stringVal = new char[data.size() + 1];
    memcpy(_field._stringVal, data.c_str(), data.size());
    _field._stringVal[data.size()] = '\0';
}

Value& Value::operator=(const Value& value)
{
    Copy(const_cast<Value&>(value));
    return *this;
}

Value& Value::operator=(unsigned char data)
{
    Reset(Type::UCHAR);
    _field._ucharVal = data;
    return *this;
}
Value& Value::operator=(unsigned short data)
{
    Reset(Type::USHORT);
    _field._ushortVal = data;
    return *this;
}
Value& Value::operator=(unsigned int data)
{
    Reset(Type::UINT);
    _field._uintVal = data;
    return *this;
}


Value& Value::operator=(unsigned long long data)
{
    Reset(Type::ULONGLONG);
    _field._ulonglongVal = data;
    return *this;
}
Value& Value::operator=(bool data)
{
    Reset(Type::BOOL);
    _field._boolVal = data;
    return *this;
}
Value& Value::operator=(char data)
{
    Reset(Type::CHAR);
    _field._charVal = data;
    return *this;
}
Value& Value::operator=(short data)
{
    Reset(Type::SHORT);
    _field._shortVal = data;
    return *this;
}
Value& Value::operator=(int data)
{
    Reset(Type::INT);
    _field._intVal = data;
    return *this;
}

Value& Value::operator=(long long data)
{
    Reset(Type::LONGLONG);
    _field._longlongVal = data;
    return *this;
}
Value& Value::operator=(float data)
{
    Reset(Type::FLOAT);
    _field._floatVal = data;
    return *this;
}
Value& Value::operator=(double data)
{
    Reset(Type::DOUBLE);
    _field._doubleVal = data;
    return *this;
}
Value& Value::operator=(const char* data)
{
    Reset(Type::PCHAR);
    size_t len = strlen(data);
    _field._pcharVal = new char[len + 1];
    memcpy(_field._pcharVal, data, len);
    _field._pcharVal[len] = '\0';
    return *this;
}
Value& Value::operator=(const std::string& data)
{
    Reset(Type::STRING);
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
    const Value &t = *this;
    return t == other;
}

bool Value::operator== (const Value& other) const
{
    if (this == &other) return true;
    if (other._type != this->_type) return false;
    if (this->IsNull()) return true;
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
    case Value::Type::POINTER: return (_field._pointer == other._field._pointer);
    default:
        break;
    }
    return false;
}

void Value::Copy(Value& other)
{
    _type = other.GetType();
    switch (_type)
    {
    case Value::Type::NONE:
        memset(&_field, 0, sizeof(_field));
        break;
    case Value::Type::UCHAR:
        _field._ucharVal = other.As<unsigned char>();
        break;
    case Value::Type::USHORT:
        _field._ushortVal = other.As<unsigned short>();
        break;
    case Value::Type::UINT:
        _field._uintVal = other.As<unsigned int>();
        break;
    case Value::Type::ULONGLONG:
        _field._ulonglongVal = other.As<unsigned long long>();
        break;
    case Value::Type::BOOL:
        _field._boolVal = other.As<bool>();
        break;
    case Value::Type::CHAR:
        _field._charVal = other.As<char>();
        break;
    case Value::Type::SHORT:
        _field._shortVal = other.As<short>();
        break;
    case Value::Type::INT:
        _field._intVal = other.As<int>();
        break;
    case Value::Type::LONGLONG:
        _field._longlongVal = other.As<long long>();
        break;
    case Value::Type::FLOAT:
        _field._floatVal = other.As<float>();
        break;
    case Value::Type::DOUBLE:
        _field._doubleVal = other.As<double>();
        break;
    case Value::Type::PCHAR:
        {
            SAFE_DELETE(_field._pcharVal);
            char* val = other.As<char*>();
            size_t len = strlen(val);
            _field._pcharVal = new char[len + 1];
            memcpy(_field._pcharVal, val, len);
            _field._pcharVal[len] = '\0';
        }
        break;
    case Value::Type::STRING:
        {
            SAFE_DELETE(_field._stringVal);
            std::string val = other.As<std::string>();
            _field._stringVal = new char[val.size() + 1];
            memcpy(_field._stringVal, val.c_str(), val.size());
            _field._stringVal[val.size()] = '\0';
        }
        break;
    case  Value::Type::POINTER:
        _field._pointer = other._field._pointer;
        break;
    default:
        break;
    }
}


void Value::Swap(Value& other)
{
    std::swap(_type, other._type);
    std::swap(_field, other._field);
}

void Value::Clear()
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


void Value::Reset(Type valType)
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




