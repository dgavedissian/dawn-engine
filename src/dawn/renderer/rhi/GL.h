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

#ifndef DW_EMSCRIPTEN
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#else
// Tell GLFW to load the OpenGL ES 3 headers.
#define GLFW_INCLUDE_ES3
#endif

#include <GLFW/glfw3.h>

#ifndef GL_BGRA
#	define GL_BGRA 0x80E1
#endif // GL_BGRA

#ifndef GL_UNSIGNED_SHORT_1_5_5_5_REV
#	define GL_UNSIGNED_SHORT_1_5_5_5_REV 0x8366
#endif // GL_UNSIGNED_SHORT_1_5_5_5_REV

#ifndef GL_UNSIGNED_SHORT_4_4_4_4_REV
#	define GL_UNSIGNED_SHORT_4_4_4_4_REV 0x8365
#endif // GL_UNSIGNED_SHORT_4_4_4_4_REV

#ifndef GL_R16
#	define GL_R16 0x822A
#endif // GL_R16

#ifndef GL_R16_SNORM
#	define GL_R16_SNORM 0x8F98
#endif // GL_R16_SNORM

#ifndef GL_RG16
#	define GL_RG16 0x822C
#endif // GL_RG16

#ifndef GL_RG16_SNORM
#	define GL_RG16_SNORM 0x8F99
#endif // GL_RG16_SNORM

#ifndef GL_RGBA16
#	define GL_RGBA16 0x805B
#endif // GL_RGBA16

#ifndef GL_RGBA16_SNORM
#	define GL_RGBA16_SNORM 0x8F9B
#endif // GL_RGBA16_SNORM

#ifndef GL_DEPTH_COMPONENT32
#	define GL_DEPTH_COMPONENT32 0x81A7
#endif // GL_DEPTH_COMPONENT32

#ifndef GL_STENCIL_INDEX
#	define GL_STENCIL_INDEX 0x1901
#endif // GL_STENCIL_INDEX

// Re-enable warnings
#if defined(DW_MSVC)
#pragma warning(pop)
#endif
