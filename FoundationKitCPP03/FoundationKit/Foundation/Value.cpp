/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.

  losemymind.libo@gmail.com

****************************************************************************/

#include <sstream>
#include <assert.h>
#include "Value.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
NS_FK_BEGIN

const Value Value::Null;

Value::Value(Type valType)
    : _type(valType)
{
    memset(&_field, 0x00, sizeof(_field));
}


Value::Value()
{
    init(Type::NONE);
}

Value::~Value()
{
    this->Clear();
}

Value::Value(const Value& other)
{
    init(Type::NONE);
    Copy(const_cast<Value&>(other));
}


Value::Value(uint8 data)
{
    init(Type::UCHAR);
    _field._ucharVal = data;
}

Value::Value(uint16 data)
{
    init(Type::USHORT);
    _field._ushortVal = data;
}

Value::Value(uint32 data)
{
    init(Type::UINT);
    _field._uintVal = data;
}

Value::Value(uint64 data)
{
    init(Type::ULONGLONG);
    _field._ulonglongVal = data;
}

Value::Value(bool data)
{
    init(Type::BOOL);
    _field._boolVal = data;
}

Value::Value(char data)
{
    init(Type::CHAR);
    _field._charVal = data;
}

Value::Value(int16 data)
{
    init(Type::SHORT);
    _field._shortVal = data;
}

Value::Value(int32 data)
{
    init(Type::INT);
    _field._intVal = data;
}

Value::Value(int64 data)
{
    init(Type::LONGLONG);
    _field._longlongVal = data;
}

Value::Value(float data)
{
    init(Type::FLOAT);
    _field._floatVal = data;
}

Value::Value(double data)
{
    init(Type::DOUBLE);
    _field._doubleVal = data;
}

Value::Value(const char* data)
{
    init(Type::PCHAR);
    size_t len = strlen(data);
    _field._pcharVal = new char[len + 1];
    memcpy(_field._pcharVal, data, len);
    _field._pcharVal[len] = '\0';
}

Value::Value(const std::string& data)
{
    init(Type::STRING);
    _field._stringVal = new char[data.size() + 1];
    memcpy(_field._stringVal, data.c_str(), data.size());
    _field._stringVal[data.size()] = '\0';
}

Value& Value::operator=(const Value& value)
{
    Copy(const_cast<Value&>(value));
    return *this;
}

Value& Value::operator=(uint8 data)
{
    Reset(Type::UCHAR);
    _field._ucharVal = data;
    return *this;
}
Value& Value::operator=(uint16 data)
{
    Reset(Type::USHORT);
    _field._ushortVal = data;
    return *this;
}
Value& Value::operator=(uint32 data)
{
    Reset(Type::UINT);
    _field._uintVal = data;
    return *this;
}


Value& Value::operator=(uint64 data)
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
Value& Value::operator=(int16 data)
{
    Reset(Type::SHORT);
    _field._shortVal = data;
    return *this;
}
Value& Value::operator=(int32 data)
{
    Reset(Type::INT);
    _field._intVal = data;
    return *this;
}

Value& Value::operator=(int64 data)
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

void Value::Move(Value& other)
{
    _type = std::move(other._type);
    _field = std::move(other._field);
    memset(&other._field, 0, sizeof(other._field));
    other._type = Type::NONE; 
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


void FoundationKit::Value::init(Type valType)
{
    _type = valType;
    memset(&_field, 0x00, sizeof(_field));
}
std::string FoundationKit::Value::ToString()
{
    std::string result;
    switch (_type)
    {
    case Value::Type::UCHAR:
        result = StringUtils::Format("%c", _field._ucharVal);
        break;
    case Value::Type::USHORT:
        result = StringUtils::Format("%d", _field._ushortVal);
        break;
    case Value::Type::UINT:
        result = StringUtils::Format("%d", _field._uintVal);
        break;
    case Value::Type::ULONGLONG:
        result = StringUtils::Format("%lld", _field._ulonglongVal);
        break;
    case Value::Type::BOOL:
        result = StringUtils::Format("%s", _field._boolVal?"true":"false");
        break;
    case Value::Type::CHAR:
        result = StringUtils::Format("%c", _field._charVal);
        break;
    case Value::Type::SHORT:
        result = StringUtils::Format("%d", _field._shortVal);
        break;
    case Value::Type::INT:
        result = StringUtils::Format("%d", _field._intVal);
        break;
    case Value::Type::LONGLONG:
        result = StringUtils::Format("%lld", _field._longlongVal);
        break;
    case Value::Type::FLOAT:
        result = StringUtils::Format("%f", _field._floatVal);
        break;
    case Value::Type::DOUBLE:
        result = StringUtils::Format("%f", _field._doubleVal);
        break;
    case Value::Type::PCHAR:
        result = _field._pcharVal;
        break;
    case Value::Type::STRING:
        result = _field._stringVal;
        break;
    case  Value::Type::POINTER:
        result = StringUtils::Format("%p", _field._pointer);
        break;
    default:
        break;
    }
    return result;
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




