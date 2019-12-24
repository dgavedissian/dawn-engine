/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "Sol.h"

namespace dw {

// Manages the Lua virtual machine and provides some helper functions
class DW_API LuaState : public Module {
public:
    DW_OBJECT(LuaState);

    LuaState(Context* context);
    virtual ~LuaState();

private:
};
}  // namespace dw
