/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include <SDL_syswm.h>

namespace dw {

uintptr_t getWindowContentViewHandle(SDL_SysWMinfo &info);
}
