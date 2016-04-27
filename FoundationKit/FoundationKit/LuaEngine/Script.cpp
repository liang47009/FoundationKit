
/****************************************************************************
Copyright (c) 2013-2014 libo.
 
losemymind.libo@gmail.com

****************************************************************************/
#include <algorithm>
#include "Script.h"
#include "FoundationKit/Foundation/Logger.h"
#include "LuaScriptHelper.h"

NS_FK_BEGIN

static int registerEvent(lua_State* L)
{
    return Script::getInstance()->registerEvent(L);
}

static int unregisterEvent(lua_State* L)
{
    return Script::getInstance()->unregisterEvent(L);
}

static int executeEvent(lua_State* L)
{
    return Script::getInstance()->executeEvent(L);
}

static int executeCppEvent(lua_State* L)
{
    return Script::getInstance()->executeCppEvent(L);
}

static const luaL_Reg scriptlib[] = 
{
    {"registerEvent", registerEvent},
    {"unregisterEvent", unregisterEvent},
    {"executeEvent", executeEvent},
    {"executeCppEvent", executeCppEvent},
    {NULL, NULL}
};



Script::Script():m_stack(nullptr)
{

}

Script::~Script()
{

}

void Script::open(LuaStack* stack)
{
    LOG_ASSERT(stack != NULL , " >>>> Invalid luaState for Script::setup ");
    m_stack = stack;
    ::luaL_register(m_stack->getLuaState(), "_G", scriptlib);
}

int Script::registerEvent( lua_State* L )
{
    ::luaL_checktype(L, 1, LUA_TSTRING);//事件名称
    ::luaL_checktype(L, 2, LUA_TFUNCTION);//Lua function
    const char* eventname = ::lua_tostring(L, 1);
    int handler = luaevent_ref_function(L, 2, 0);

    auto iter = m_eventMap.find(eventname);
    if (iter == m_eventMap.end())
    {
        vector< int > vecHandler;
        vecHandler.push_back(handler);
        m_eventMap.insert(pair<std::string, vector<int> >(eventname, vecHandler));
    }
    else if(std::find(iter->second.begin(), iter->second.end(), handler) == iter->second.end())
    {
        iter->second.push_back(handler);
    }
    return 0;
}

int Script::unregisterEvent( lua_State* L )
{
    ::luaL_checktype(L, 1, LUA_TSTRING);//事件名称
    ::luaL_checktype(L, 2, LUA_TFUNCTION);//Lua function
    const char* eventname = ::lua_tostring(L, 1);
    int handler = luaevent_ref_function(L, 2, 0);

    auto iter = m_eventMap.find(eventname);
    if (iter != m_eventMap.end())
    {
        auto handlerIter = std::find(iter->second.begin(), iter->second.end(), handler);
        if (handlerIter != iter->second.end())
        {
            luaevent_remove_function_by_refid(L, handler);
            iter->second.erase(handlerIter);
        }
    }
    return 0;
}

int Script::executeEvent( lua_State* L )
{
    ::luaL_checktype(L, 1, LUA_TSTRING);//EVENT
    const char* eventname = ::lua_tostring(L, 1);
    LuaValueArray arglist;
    ::lua_gettop(L);
    parseParams(L, arglist, 2);
    int ret = executeEvent(eventname, arglist);
    return ret;
}

int Script::executeEvent(const char* pEventName, LuaValueArray& params)
{
    LOG_ASSERT(pEventName != NULL, " >>>> Invalid pEventName for Script::executeEvent ");
    auto iter = m_eventMap.find(pEventName);
    if (iter != m_eventMap.end())
    {
        vector< int > tempHandlerList(iter->second.begin(), iter->second.end());
        for (auto handler : tempHandlerList)
        {
            m_stack->pushString(pEventName);
            m_stack->pushLuaValueArray(params);
            m_stack->executeFunctionByHandler(handler, static_cast<int>(params.size()+1));
            m_stack->clean();
        }
    }
    return 0;
}


void Script::parseParams(lua_State* L, LuaValueArray& result, int first, int mode /*= 1*/)
{
    int count = lua_gettop(L);
    int index = 0;
    for(int i = first; i <= count; i++)
    {
        index = i*mode;

        int type = ::lua_type(L, index);
        switch(type)
        {
        case LUA_TNUMBER:
            result.push_back(LuaValue::doubleValue(::lua_tonumber(L, index)));
            break;
        case LUA_TSTRING:
            result.push_back(LuaValue::stringValue(::lua_tostring(L, index)));
            break;
        case LUA_TBOOLEAN:
            result.push_back(LuaValue::booleanValue(::lua_toboolean(L, index) != 0));
            break;
        case LUA_TNIL:
            result.push_back(LuaValue());
            break;
        default:
            break;
        }
    }
}


void Script::PushLuaParam(lua_State* L, const LuaValue& var)
{
    switch (var.getType())
    {
    case LuaValueTypeInt:
        ::lua_pushinteger(L, var.intValue());
        break;
    case LuaValueTypeFloat:
        ::lua_pushnumber(L, var.floatValue());
        break;
    case LuaValueTypeBoolean:
        ::lua_pushboolean(L,var.booleanValue());
        break;
    case LuaValueTypeString:
        ::lua_pushstring(L, var.stringValue().c_str());
        break;
    default:
        LOG_INFO(" >>> Not support value type ");
        break;
    }
}

int Script::registerCppEvent( const char* pEventName, cpp_callback callback )
{
    auto iter = m_funMap.find(pEventName);
    if (iter == m_funMap.end())
    {
        m_funMap.insert(pair<string, cpp_callback >(pEventName, callback));
    }
    return 0;
}

int Script::unregisterCppEvent( const char* pEventName, cpp_callback callback )
{
    auto iter = m_funMap.find(pEventName);
    if (iter != m_funMap.end())
    {
        m_funMap.erase(iter);
    }
    return 0;
}

int Script::executeCppEvent( lua_State* L )
{
    ::luaL_checktype(L, 1, LUA_TSTRING);//EVENT
    const char* eventname = ::lua_tostring(L, 1);
    LuaValueArray arglist;
    ::lua_gettop(L);
    parseParams(L, arglist, 2);
    LuaValueArray result;
    int ret = executeCppEvent(eventname, arglist, result);

    for (auto luavalue : result)
    {
        PushLuaParam(L, luavalue);
    }
    return ret;
}

int Script::executeCppEvent(const char* pEventName, LuaValueArray& args, LuaValueArray& result)
{
    LOG_ASSERT(pEventName != NULL, " >>>> Invalid pEventName for Script::executeEvent ");
    auto iter = m_funMap.find(pEventName);
    if (iter != m_funMap.end())
    {
        iter->second(args, result);
    }
    else
    {
        LOG_INFO("The function name [%s] is not found !",pEventName);
    }
    return 0;
}

NS_FK_END