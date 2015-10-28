#pragma once


extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

    extern int lua_loader(lua_State *L);
}