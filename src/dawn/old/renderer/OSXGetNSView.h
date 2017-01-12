/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include <SDL_syswm.h>

namespace dw {

uintptr_t getWindowContentViewHandle(SDL_SysWMinfo &info);
}
