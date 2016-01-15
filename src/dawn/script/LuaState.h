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
    void ExecuteString(const String& str);

    // Execute a file
    void ExecuteFile(const String& filename);

    // Get an object from the global namespace
    LuaRef GetGlobal(const String& name);

    // Get the global namespace and begin binding
    Namespace Bind();

    // Accessors
    lua_State* GetState();

private:
    lua_State* mL;
};

NAMESPACE_END
