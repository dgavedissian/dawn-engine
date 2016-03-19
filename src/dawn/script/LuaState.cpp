/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "LuaState.h"

NAMESPACE_BEGIN

void l_logWrite(const String& s)
{
    LOG << s;
}

LuaState::LuaState() : mL(nullptr)
{
    // Create the vm
    mL = luaL_newstate();
    luaL_openlibs(mL);

    // Bind a new print function
    bind().addFunction("_logWrite", l_logWrite);

    // Redirect Lua printing to the Log
    executeString(
        "function print(...)\n"
        "  local arg = {...}\n"
        "  local str\n"
        "  str = ''\n"
        "  for i = 1, #arg do\n"
        "    if str ~= '' then str = str .. '\t' end\n"
        "    str = str .. tostring(arg[i])\n"
        "  end\n"
        "  str = str .. '\\n'\n"
        "  _logWrite(str)\n"
        "end\n");
}

LuaState::~LuaState()
{
    lua_close(mL);
}

void LuaState::executeString(const String& str)
{
    luaL_loadstring(mL, str.c_str());
    int result = lua_pcall(mL, 0, 0, 0);
    if (result != 0)
    {
        LOG << lua_tostring(mL, -1);
    }
}

void LuaState::executeFile(const String& filename)
{
    String fullFilename = "Media/" + filename;
    luaL_loadfile(mL, fullFilename.c_str());
    int result = lua_pcall(mL, 0, 0, 0);
    if (result != 0)
    {
        LOG << lua_tostring(mL, -1);
    }
}

luabridge::LuaRef LuaState::getGlobal(const String& name)
{
    return luabridge::getGlobal(mL, name.c_str());
}

luabridge::Namespace LuaState::bind()
{
    return luabridge::getGlobalNamespace(mL);
}

lua_State* LuaState::getState()
{
    return mL;
}

NAMESPACE_END
