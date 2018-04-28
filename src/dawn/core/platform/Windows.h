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

#ifdef APIENTRY
#undef APIENTRY
#endif
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif

// Re-enable warnings
#if defined(DW_MSVC)
#pragma warning(pop)
#endif
