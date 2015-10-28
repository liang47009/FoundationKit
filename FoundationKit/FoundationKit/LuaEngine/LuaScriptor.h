/****************************************************************************
Copyright (c) 2013-2014 libo.
 
losemymind.libo@gmail.com

****************************************************************************/

#pragma once

#include "LuaScriptHelper.h"

NS_FK_BEGIN

class Scriptor
{
private:

    lua_State* m_pLuaState;

public:

    Scriptor():m_pLuaState(luaL_newstate())
    {
        //open the libraries
        luaL_openlibs(m_pLuaState);
    }

    ~Scriptor(){lua_close(m_pLuaState);}

    void runScriptFile(const char* scriptName)
    {
        runLuaScript(m_pLuaState, scriptName);
    }

    void runScriptString(const char* scriptString)
    {
        runLuaString(m_pLuaState, scriptString);
    }

    lua_State* getState(){return m_pLuaState;}


    int getInt(const char* variableName)
    {
        return popLuaNumber<int>(m_pLuaState, variableName);
    }

    int getInt(const char* pTableName, const char* key)
    {
        return popLuaTableNumberVaue<int>(m_pLuaState, pTableName, key);
    }

    int getInt(const char* pTableName, int index)
    {
        return popLuaTableNumberVaue<int>(m_pLuaState, pTableName, index);
    }

    float getFloat(const char* variableName)
    {
        return popLuaNumber<float>(m_pLuaState, variableName);
    }

    float getFloat(char* pTableName, char* key)
    {
        return popLuaTableNumberVaue<float>(m_pLuaState, pTableName, key);
    }

    float getFloat(char* pTableName, int index)
    {
        return popLuaTableNumberVaue<float>(m_pLuaState, pTableName, index);
    }

    double getDouble(char* variableName)
    {
        return popLuaNumber<double>(m_pLuaState, variableName);
    }

    double getDouble(char* pTableName, char* key)
    {
        return popLuaTableNumberVaue<double>(m_pLuaState, pTableName, key);
    }

    double getDouble(char* pTableName, int index)
    {
        return popLuaTableNumberVaue<double>(m_pLuaState, pTableName, index);
    }

    std::string getString(char* variableName)
    {
        return popLuaString(m_pLuaState, variableName);
    }

    std::string getString(char* pTableName, char* key)
    {
        return popLuaTableStringVaue(m_pLuaState, pTableName, key);
    }

    std::string getString(char* pTableName, int index)
    {
        return popLuaTableStringVaue(m_pLuaState, pTableName, index);
    }

    bool getBool(char* variableName)
    {
        return popLuaBool(m_pLuaState, variableName);
    }

    bool getBool(char* pTableName, char* key)
    {
        return popLuaTableBoolVaue(m_pLuaState, pTableName, key);
    }

    bool getBool(char* pTableName, int index)
    {
        return popLuaTableBoolVaue(m_pLuaState, pTableName, index);
    }

};
NS_FK_END