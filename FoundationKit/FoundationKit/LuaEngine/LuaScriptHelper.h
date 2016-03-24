/****************************************************************************
Copyright (c) 2013-2014 libo.
 
losemymind.libo@gmail.com

****************************************************************************/
#pragma once

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <string>
#include "FoundationKit/FoundationKitDefines.h"
#include "FoundationKit/Foundation/Logger.h"
#include "FoundationKit/Foundation/StringUtils.h"

NS_FK_BEGIN

#define TOLUA_REFID_FUNCTION_MAPPING "TOLUA_REFID_FUNCTION_MAPPING"
static inline void luaevent_open(lua_State * L)
{
    lua_pushstring(L, TOLUA_REFID_FUNCTION_MAPPING);
    lua_newtable(L);
    lua_rawset(L, LUA_REGISTRYINDEX);
}

static int luaevent_ref_function(lua_State* L, int lo, int def)
{
    __unused_arg(def);
    // function at lo
    if (!lua_isfunction(L, lo)) return 0;

    uintptr_t s_function_ref_id = (uintptr_t)(lua_topointer(L, lo));;

    lua_pushstring(L, TOLUA_REFID_FUNCTION_MAPPING);
    lua_rawget(L, LUA_REGISTRYINDEX);                           /* stack: fun ... refid_fun */
    lua_pushinteger(L, s_function_ref_id);                      /* stack: fun ... refid_fun refid */
    lua_pushvalue(L, lo);                                       /* stack: fun ... refid_fun refid fun */

    lua_rawset(L, -3);                  /* refid_fun[refid] = fun, stack: fun ... refid_ptr */
    lua_pop(L, 1);                                              /* stack: fun ... */

    return static_cast<int>(s_function_ref_id);
}

static void luaevent_remove_function_by_refid(lua_State* L, int refid)
{
    lua_pushstring(L, TOLUA_REFID_FUNCTION_MAPPING);
    lua_rawget(L, LUA_REGISTRYINDEX);                           /* stack: ... refid_fun */
    lua_pushinteger(L, refid);                                  /* stack: ... refid_fun refid */
    lua_pushnil(L);                                             /* stack: ... refid_fun refid nil */
    lua_rawset(L, -3);                  /* refid_fun[refid] = fun, stack: ... refid_ptr */
    lua_pop(L, 1);                                              /* stack: ... */

    // luaL_unref(L, LUA_REGISTRYINDEX, refid);
}

static inline void luaevent_get_function_by_refid(lua_State* L, int refid)
{
    lua_pushstring(L, TOLUA_REFID_FUNCTION_MAPPING);
    lua_rawget(L, LUA_REGISTRYINDEX);                           /* stack: ... refid_fun */
    lua_pushinteger(L, refid);                                  /* stack: ... refid_fun refid */
    lua_rawget(L, -2);                                          /* stack: ... refid_fun fun */
    lua_remove(L, -2);                                          /* stack: ... fun */
}

/** 
 * Run lua script file.
 * @param[in] L         The lua state ( luaL_newstate() ).
 * @param[in] filePath  Lua file full path.
  */
inline bool runLuaScript(lua_State* L, const char* filePath)
{
    bool success = true;
    if (int error = luaL_dofile(L, filePath) != 0)
    {
        success = false;
        LOG_INFO(("ERROR(" + StringUtils::to_string(error) + "): Problem with lua script file " + filePath).c_str());
    }
    return success;
}

inline bool runLuaString(lua_State* L, const char* strBuf)
{
    bool success = true;
    if (int error = luaL_dostring(L, strBuf) != 0)
    {
        success = false;
        LOG_INFO(("ERROR(" + StringUtils::to_string(error) + "): Problem with lua script file " + strBuf).c_str());
    }
    return success;
}

/** 
 * Get number(int, float, double, ...) value from lua.
 * @param[in] L      The lua state.
 * @param[in] name   Variable name.
 */
template< class Number>
inline Number popLuaNumber(lua_State* L, const char* name)
{
    if (!name)
    {
        LOG_ERROR("ERROR: The param name is nullptr");
        return Number();
    }
    lua_settop(L, 0);
    lua_getglobal(L, name);

    // Check that the variable is the correct type. 
    // If it is not throw an exception.
    if (!lua_isnumber(L, 1))
    {
        std::string err("<popLuaNumber> Cannot retrieve:");
        throw std::runtime_error(err + name);
    }

    Number val = static_cast<Number>(lua_tonumber(L, 1));
    lua_pop(L, 1);
    return val;
}

/** 
 * Get string value from lua.
 * @param[in] L      The lua state.
 * @param[in] name   Variable name.
 */
inline std::string popLuaString(lua_State* L, const char* name)
{
    if (!name)
    {
        LOG_INFO("ERROR: The param name is nullptr");
        return std::string();
    }
    lua_settop(L, 0);
    lua_getglobal(L, name);

    // Check that the variable is the correct type. 
    // If it is not throw an exception.
    if (!lua_isstring(L, 1))
    {
        std::string err("<popLuaString> Cannot retrieve:");
        throw std::runtime_error(err + name);
    }

    std::string val = lua_tostring(L, 1);
    lua_pop(L, 1);
    return val;
}

/** 
 * Get bool value from lua.
 * @param[in] L      The lua state.
 * @param[in] name   Variable name.
 */
inline bool popLuaBool(lua_State* L, const char* name)
{
    if (!name)
    {
        LOG_INFO("ERROR: The param name is nullptr");
        return false;
    }
    lua_settop(L, 0);
    lua_getglobal(L, name);

    // Check that the variable is the correct type. 
    // If it is not throw an exception.
    if (!lua_isboolean(L, 1))
    {
        std::string err("<popLuaBool> Cannot retrieve:");
        throw std::runtime_error(err + name);
    }

    bool val = lua_toboolean(L, 1) != 0;
    lua_pop(L, 1);
    return val;
}

/** 
 * Get number value from lua table.
 * @param[in] L              The lua state.
 * @param[in] pszTableName   Lua table name
 * @param[in] key            Table field name.
 */
template< class Number >
inline Number popLuaTableNumberVaue(lua_State* L, const char* pszTableName, const char* key)
{
    lua_getglobal(L, pszTableName);
    lua_pushstring(L, key);
    lua_gettable(L, -2);

    if (!lua_isnumber(L, -1))
    {
        std::string err("<popLuaTableNumberVaue> Cannot retrieve:");
        throw std::runtime_error(err + pszTableName);
    }

    Number val = static_cast<Number>(lua_tonumber(L, -1));
    lua_pop(L, 2);
    return val;
}

/** 
 * Get string value from lua table.
 * @param[in] L              The lua state.
 * @param[in] pszTableName   Lua table name
 * @param[in] key            Table field name.
 */
inline std::string popLuaTableStringVaue(lua_State* L, const char* pszTableName, const char* key)
{
    lua_getglobal(L, pszTableName);
    lua_pushstring(L, key);
    lua_gettable(L, -2);

    if (!lua_isstring(L, -1))
    {
        std::string err("<popLuaTableStringVaue> Cannot retrieve:");
        throw std::runtime_error(err + pszTableName);
    }

    std::string val = lua_tostring(L, -1);
    lua_pop(L, 2);
    return val;
}

/** 
 * Get bool value from lua table.
 * @param[in] L              The lua state.
 * @param[in] pszTableName   Lua table name
 * @param[in] key            Table field name.
 */
inline bool popLuaTableBoolVaue(lua_State* L, const char* pszTableName, const char* key)
{
    lua_getglobal(L, pszTableName);
    lua_pushstring(L, key);
    lua_gettable(L, -2);

    if (!lua_isboolean(L, -1))
    {
        std::string err("<popLuaTableBoolVaue> Cannot retrieve:");
        throw std::runtime_error(err + pszTableName);
    }

    bool val = lua_toboolean(L, -1) != 0;
    lua_pop(L, 2);
    return val;
}

/** 
 * Get number value from lua table.
 * @param[in] L              The lua state.
 * @param[in] pszTableName   Lua table name
 * @param[in] index          Table value index.
 */
template< class Number >
inline Number popLuaTableNumberVaue(lua_State* L, const char* pszTableName, int index)
{
    lua_getglobal(L, pszTableName);
    lua_rawgeti(L, -1, index);
    if (!lua_isnumber(L, -1))
    {
        std::string err("<popLuaTableNumberVaue> Cannot retrieve:");
        throw std::runtime_error(err + pszTableName);
    }
    Number val = static_cast<Number>(lua_tonumber(L, -1));
    lua_pop(L, 2);
    return val;
}

/** 
 * Get string value from lua table.
 * @param[in] L              The lua state.
 * @param[in] pszTableName   Lua table name
 * @param[in] index          Table value index.
 */
inline std::string popLuaTableStringVaue(lua_State* L, const char* pszTableName, int index)
{
    lua_getglobal(L, pszTableName);
    lua_rawgeti(L, -1, index);
    if (!lua_isstring(L, -1))
    {
        std::string err("<popLuaTableStringVaue> Cannot retrieve:");
        throw std::runtime_error(err + pszTableName);
    }
    std::string val = lua_tostring(L, -1);
    lua_pop(L, 2);
    return val;
}

/** 
 * Get bool value from lua table.
 * @param[in] L              The lua state.
 * @param[in] pszTableName   Lua table name
 * @param[in] index          Table value index.
 */
inline bool popLuaTableBoolVaue(lua_State* L, const char* pszTableName, int index)
{
    lua_getglobal(L, pszTableName);
    lua_rawgeti(L, -1, index);
    if (!lua_isboolean(L, -1))
    {
        std::string err("<popLuaTableBoolVaue> Cannot retrieve:");
        throw std::runtime_error(err + pszTableName );
    }
    bool val = lua_toboolean(L, -1) != 0;
    lua_pop(L, 2);
    return val;
}

NS_FK_END