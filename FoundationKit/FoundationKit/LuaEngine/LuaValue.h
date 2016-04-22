#pragma once

#include <string>
#include <map>
#include <list>
#include "FoundationKit/GenericPlatformMacros.h"

extern "C" {
#include "lua.h"
}

NS_FK_BEGIN

typedef int LUA_FUNCTION;
typedef int LUA_TABLE;
typedef int LUA_STRING;

class LuaValue;

typedef std::map<std::string, LuaValue>             LuaValueDict;
typedef LuaValueDict::const_iterator                LuaValueDictIterator;
typedef std::list<LuaValue>                         LuaValueArray;
typedef LuaValueArray::const_iterator               LuaValueArrayIterator;

typedef enum {
	LuaValueTypeInt,
	LuaValueTypeFloat,
	LuaValueTypeDouble,
	LuaValueTypeBoolean,
	LuaValueTypeString,
	LuaValueTypeDict,
	LuaValueTypeArray,
} LuaValueType;

typedef union {
	int                 intValue;
	float               floatValue;
	double              doubleValue;
	bool                booleanValue;
	std::string*        stringValue;
	LuaValueDict*       dictValue;
	LuaValueArray*      arrayValue;
} LuaValueField;

class LuaValue
{
public:
	static const LuaValue intValue(const int intValue);
	static const LuaValue floatValue(const float floatValue);
	static const LuaValue doubleValue(const double doubleValue);
	static const LuaValue booleanValue(const bool booleanValue);
	static const LuaValue stringValue(const char* stringValue);
	static const LuaValue stringValue(const std::string& stringValue);
	static const LuaValue dictValue(const LuaValueDict& dictValue);
	static const LuaValue arrayValue(const LuaValueArray& arrayValue);

	LuaValue(void)
		: _type(LuaValueTypeInt)
	{
		memset(&_field, 0, sizeof(_field));
	}
	LuaValue(const LuaValue& rhs);
	LuaValue& operator=(const LuaValue& rhs);
	~LuaValue(void);

	const LuaValueType getType(void) const {
		return _type;
	}

	int intValue(void) const {
		return _field.intValue;
	}

	double doubleValue(void) const {
		return _field.doubleValue;
	}

	float floatValue(void) const {
		return _field.floatValue;
	}

	bool booleanValue(void) const {
		return _field.booleanValue;
	}

	const std::string& stringValue(void) const {
		return *_field.stringValue;
	}

	const LuaValueDict& dictValue(void) const {
		return *_field.dictValue;
	}

	const LuaValueArray& arrayValue(void) const {
		return *_field.arrayValue;
	}

private:
	LuaValueField   _field;
	LuaValueType    _type;

	void copy(const LuaValue& rhs);
};

NS_FK_END
