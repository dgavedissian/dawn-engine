/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

namespace dw {

// Manages the Lua virtual machine and provides some helper functions
class DW_API LuaState : public Object {
public:
    DW_OBJECT(LuaState);

    LuaState(Context* context);
    virtual ~LuaState();

private:
};
}
