/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include <SDL_syswm.h>

namespace dw {

uintptr_t getWindowContentViewHandle(SDL_SysWMinfo& info);
}
