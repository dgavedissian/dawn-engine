/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#pragma once

#include "Sol.h"
#include "core/io/InputStream.h"

namespace dw {

// Manages a Lua virtual machine, with the "dw" library set.
class DW_API LuaVM : public Object {
public:
    DW_OBJECT(LuaVM);

    LuaVM(Context* context);
    virtual ~LuaVM() = default;

    // Get a reference to the internal state.
    sol::state& state();

    // Execute a file from a string.
    Result<void> execute(const String& text);

    // Execute a file from an input stream.
    Result<void> execute(InputStream& is);

private:
    sol::state state_;
};
}  // namespace dw
