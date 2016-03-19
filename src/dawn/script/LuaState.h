/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

NAMESPACE_BEGIN

// Manages the Lua virtual machine and provides some helper functions
class DW_API LuaState
{
public:
    LuaState();
    virtual ~LuaState();

    // Execute a string
    void executeString(const String& str);

    // Execute a file
    void executeFile(const String& filename);

    // Get an object from the global namespace
    LuaRef getGlobal(const String& name);

    // Get the global namespace and begin binding
    Namespace bind();

    // Accessors
    lua_State* getState();

private:
    lua_State* mL;
};

NAMESPACE_END
