/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "LuaVM.h"

namespace dw {
namespace {
void redirectLuaPrintToLogger(Context& context, sol::state& state) {
    state.script(R"""(
function print(...)
  local arg = {...}
  local str
  str = ''
  for i = 1, #arg do
    if str ~= '' then str = str .. '\t' end
    str = str .. tostring(arg[i])
  end
  str = str .. '\n'
  __dw_write_log(debug.getinfo(2).short_src, str)
end)""");
}

void registerWorldLib(sol::state& state) {
}
}  // namespace

LuaVM::LuaVM(Context* context) : Object(context) {
    state_.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::string,
                          sol::lib::os, sol::lib::math, sol::lib::table, sol::lib::debug,
                          sol::lib::bit32);
    state_.set_function("__dw_write_log", [this](const char* caller, const char* str) {
        log().info("%s: %s", caller, str);
    });
    redirectLuaPrintToLogger(*context, state_);
    registerWorldLib(state_);
}

sol::state& LuaVM::state() {
    return state_;
}

Result<void> LuaVM::execute(const String& text) {
    try {
        state_.script(text);
    } catch (const sol::error& e) {
        return makeError(e.what());
    }
    return {};
}

Result<void> LuaVM::execute(InputStream& is) {
    auto read_result = is.readAll();
    if (!read_result) {
        return makeError(read_result.error());
    }
    return execute(String(static_cast<const char*>(static_cast<void*>(read_result->data())),
                          read_result->size()));
}
}  // namespace dw
