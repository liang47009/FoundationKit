
#include "LuaValue.h"


NS_FK_BEGIN

const LuaValue LuaValue::intValue(const int intValue)
{
	LuaValue value;
	value._type = LuaValueTypeInt;
	value._field.intValue = intValue;
	return value;
}

const LuaValue LuaValue::floatValue(const float floatValue)
{
	LuaValue value;
	value._type = LuaValueTypeFloat;
	value._field.floatValue = floatValue;
	return value;
}

const LuaValue LuaValue::doubleValue(const double doubleValue)
{
	LuaValue value;
	value._type = LuaValueTypeDouble;
	value._field.doubleValue = doubleValue;
	return value;
}


const LuaValue LuaValue::booleanValue(const bool booleanValue)
{
	LuaValue value;
	value._type = LuaValueTypeBoolean;
	value._field.booleanValue = booleanValue;
	return value;
}

const LuaValue LuaValue::stringValue(const char* stringValue)
{
	LuaValue value;
	value._type = LuaValueTypeString;
	value._field.stringValue = new std::string(stringValue ? stringValue : "");
	return value;
}

const LuaValue LuaValue::stringValue(const std::string& stringValue)
{
	LuaValue value;
	value._type = LuaValueTypeString;
	value._field.stringValue = new std::string(stringValue);
	return value;
}

const LuaValue LuaValue::dictValue(const LuaValueDict& dictValue)
{
	LuaValue value;
	value._type = LuaValueTypeDict;
	value._field.dictValue = new (std::nothrow) LuaValueDict(dictValue);
	return value;
}

const LuaValue LuaValue::arrayValue(const LuaValueArray& arrayValue)
{
	LuaValue value;
	value._type = LuaValueTypeArray;
	value._field.arrayValue = new (std::nothrow) LuaValueArray(arrayValue);
	return value;
}

LuaValue::LuaValue(const LuaValue& rhs)
{
	copy(rhs);
}

LuaValue& LuaValue::operator=(const LuaValue& rhs)
{
	if (this != &rhs) copy(rhs);
	return *this;
}

LuaValue::~LuaValue(void)
{
	if (_type == LuaValueTypeString)
	{
		delete _field.stringValue;
	}
	else if (_type == LuaValueTypeDict)
	{
		delete _field.dictValue;
	}
	else if (_type == LuaValueTypeArray)
	{
		delete _field.arrayValue;
	}
}

void LuaValue::copy(const LuaValue& rhs)
{
	memcpy(&_field, &rhs._field, sizeof(_field));
	_type = rhs._type;
	if (_type == LuaValueTypeString)
	{
		_field.stringValue = new std::string(*rhs._field.stringValue);
	}
	else if (_type == LuaValueTypeDict)
	{
		_field.dictValue = new (std::nothrow) LuaValueDict(*rhs._field.dictValue);
	}
	else if (_type == LuaValueTypeArray)
	{
		_field.arrayValue = new (std::nothrow) LuaValueArray(*rhs._field.arrayValue);
	}
}

NS_FK_END
