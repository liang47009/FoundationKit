#pragma once
#include "FoundationKit/FoundationKitDefines.h"


extern "C" {
#include "lua.h"
}

#include "LuaStack.h"
#include "FoundationKit/Foundation/Singleton.h"

NS_FK_BEGIN

class LuaEngine final : public Singleton<LuaEngine>
{
public:
    // Get LuaEngine instance.
    //static LuaEngine* getInstance(void);

   
    // When application exit, must be call this method release LuaEngine;
    static void  destroyInstance();

    virtual ~LuaEngine(void);

    LuaStack::Pointer getLuaStack(void) {
        return _stack;
    }

    /**
    @brief Add a path to find lua files in
    @param path to be added to the Lua path
    */
    virtual void addSearchPath(const char* path);

    /**
    @brief Add lua loader, now it is used on android
    */
    virtual void addLuaLoader(lua_CFunction func);

    /**
    @brief reload script code contained in the given string.
    @param moduleFileName String object holding the filename of the script file that is to be executed
    @return 0 if the string is excuted correctly.
    @return other if the string is excuted wrongly.
    */
    virtual int reload(const char* moduleFileName);

    /**
    @brief Remove Lua function reference
    */
    virtual void removeScriptHandler(int nHandler);

    /**
    @brief Execute script code contained in the given string.
    @param codes holding the valid script code that should be executed.
    @return 0 if the string is excuted correctly.
    @return other if the string is excuted wrongly.
    */
    virtual int executeString(const char* codes);

    /**
    @brief Execute a script file.
    @param filename String object holding the filename of the script file that is to be executed
    */
    virtual int executeScriptFile(const char* filename);

    /**
    @brief Execute a scripted global function.
    @brief The function should not take any parameters and should return an integer.
    @param functionName String object holding the name of the function, in the global script environment, that is to be executed.
    @return The integer value returned from the script function.
    */
    virtual int executeGlobalFunction(const char* functionName);
    virtual int executeGlobalFunction(const char* functionName, const LuaValueArray& params);
    virtual int executeEvent(int handler, const char* pEventName, const LuaValueArray& params);

private:
    friend class Singleton < LuaEngine > ;
    LuaEngine(void);
    LuaStack::Pointer _stack;
};

NS_FK_END
