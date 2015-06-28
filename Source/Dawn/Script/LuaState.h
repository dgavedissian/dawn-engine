/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

NAMESPACE_BEGIN

// Manages the Lua virtual machine and provides some helper functions
class DW_API LuaState : public Singleton<LuaState>
{
public:
    LuaState();
    virtual ~LuaState();

    // Execute a string
    void ExecuteString(const string& str);

    // Execute a file
    void ExecuteFile(const string& filename);

    // Get an object from the global namespace
    LuaRef GetGlobal(const string& name);

    // Get the global namespace and begin binding
    Namespace Bind();

    // Accessors
    lua_State* GetState();

private:
    lua_State* mL;
};

NAMESPACE_END
