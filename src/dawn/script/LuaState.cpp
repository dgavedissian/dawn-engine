/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "LuaState.h"

namespace dw {

LuaState::LuaState(Context* context) : Module(context) {
    // Redirect Lua printing to the Log
    /*
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
        */
}

LuaState::~LuaState() {
}
}  // namespace dw
