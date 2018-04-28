/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

// Mark this header as a system header
#if defined(DW_GCC) || defined(DW_CLANG)
#pragma GCC system_header
#elif defined(DW_MSVC)
#pragma warning(push, 0)
#endif

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

// Re-enable warnings
#if defined(DW_MSVC)
#pragma warning(pop)
#endif
