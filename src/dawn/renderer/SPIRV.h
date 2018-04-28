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

#include <spirv_cross/spirv_glsl.hpp>
#include <spirv_cross/spirv_hlsl.hpp>

// Re-enable warnings
#if defined(DW_MSVC)
#pragma warning(pop)
#endif
