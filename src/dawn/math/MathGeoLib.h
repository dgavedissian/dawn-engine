/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

// Mark this header as a system header
#if defined(DW_GCC) || defined(DW_CLANG)
#pragma GCC system_header
#elif defined(DW_MSVC)
#pragma warning(push, 0)
#endif

// MathGeoLib
#define MATH_ENABLE_STL_SUPPORT
#define MATH_ENABLE_UNCOMMON_OPERATIONS
#define MATH_BULLET_INTEROP
#define LOGGING_SUPPORT_DISABLED
#include <MathGeoLib.h>
#ifdef LOG
#undef LOG
#endif

// Re-enable warnings
#if defined(DW_MSVC)
#pragma warning(pop)
#endif