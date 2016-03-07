/****************************************************************************
Copyright (c) 2013-2014 libo.

 losemymind.libo@gmail.com
 ****************************************************************************/

#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
extern "C"
{
#include "lua.h"
#include "lauxlib.h"
};
#include "LuaStack.h"
#include "FoundationKit/Foundation/Singleton.h"
#include "FoundationKit/Base/Value.h"
#include "LuaValue.h"


using namespace std;

#define CPP_CALLBACK(__selector__) std::bind(&__selector__, std::placeholders::_1, std::placeholders::_2)

NS_FK_BEGIN

/** 
 * Lua interface
 * script.registerEvent("functionName", function)
 * script.unregisterEvent("functionName", function)
 * script.executeEvent("functionName", ...)
 * script.executeCppEvent("functionName")
 * C++ interface
 * registerCppEvent("functionName", cpp_callback)
 * unregisterCppEvent("functionName", cpp_callback)
 * executeCppEvent("functionName", args, result)
 * executeEvent("functionName",params)
 */
class Script : public Singleton<Script>
{
	typedef std::function<void(LuaValueArray& args, LuaValueArray& result)>  cpp_callback;
	typedef unordered_map<string, vector<int> > EventMap;
	typedef unordered_map<string, cpp_callback >  FunMap;

public:
	Script();
	~Script();
	void           open(LuaStack* stack);
	int            registerEvent(lua_State* L);
	int            unregisterEvent(lua_State* L);
	int            executeEvent(lua_State* L);
	int            executeEvent(const char* pEventName, LuaValueArray& params);

	int            registerCppEvent(const char* pEventName, cpp_callback callback);
	int            unregisterCppEvent(const char* pEventName, cpp_callback callback);
	int            executeCppEvent(lua_State* L);
	int            executeCppEvent(const char* pEventName, LuaValueArray& args, LuaValueArray& result);

protected:

	void           parseParams(lua_State* L, LuaValueArray& result, int first, int mode = 1);
	void           PushLuaParam(lua_State* L, const LuaValue& var);
private:
	LuaStack*           m_stack;
	EventMap            m_eventMap;
	FunMap              m_funMap;
};

#define  theScript Script::getInstance()

NS_FK_END