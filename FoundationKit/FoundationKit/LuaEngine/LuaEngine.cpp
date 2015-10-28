
#include "LuaEngine.h"
#include "Script.h"

NS_FK_BEGIN

LuaEngine::LuaEngine(void)
    : _stack(nullptr)
{
    _stack = LuaStack::create();
    theScript->open(_stack.get());
}

LuaEngine::~LuaEngine(void)
{

}

void LuaEngine::addSearchPath(const char* path)
{
    _stack->addSearchPath(path);
}

void LuaEngine::addLuaLoader(lua_CFunction func)
{
    _stack->addLuaLoader(func);
}

void LuaEngine::removeScriptHandler(int nHandler)
{
    _stack->removeScriptHandler(nHandler);
}

int LuaEngine::executeString(const char *codes)
{
    int ret = _stack->executeString(codes);
    _stack->clean();
    return ret;
}

int LuaEngine::executeScriptFile(const char* filename)
{
    int ret = _stack->executeScriptFile(filename);
    _stack->clean();
    return ret;
}

int LuaEngine::executeGlobalFunction(const char* functionName)
{
    int ret = _stack->executeGlobalFunction(functionName);
    _stack->clean();
    return ret;
}

int LuaEngine::executeGlobalFunction(const char* functionName, const LuaValueArray& params)
{
    int ret = _stack->executeGlobalFunction(functionName, params);
    _stack->clean();
    return ret;
}


int LuaEngine::executeEvent(int handler, const char* pEventName, const LuaValueArray& params)
{
    _stack->pushString(pEventName);
    _stack->pushLuaValueArray(params);
    int ret = _stack->executeFunctionByHandler(handler, static_cast<int>(params.size()+1));
    _stack->clean();
    return ret;
}

int LuaEngine::reload(const char* moduleFileName)
{
    return _stack->reload(moduleFileName);
}



NS_FK_END
