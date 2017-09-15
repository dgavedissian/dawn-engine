/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

// Determine build type.
#ifndef NDEBUG
#define DW_DEBUG
#endif

// Determine architecture.
#if defined(__x86_64__) || defined(_M_X64)
#define DW_ARCH_64
#else
#define DW_ARCH_32
#endif

// Determine platform.
#define DW_WIN32 0
#define DW_MACOS 1
#define DW_LINUX 2
#if defined(__WIN32__) || defined(_WIN32)
#define DW_PLATFORM DW_WIN32
#elif defined(__APPLE_CC__)
#define DW_PLATFORM DW_MACOS
#else
#define DW_PLATFORM DW_LINUX
#endif

// Determine compiler.
// Note that clang-format breaks the libstdc++ detection when using clang.
// clang-format off
#define __VERSION(major, minor, patch) (major * 1000 + minor * 100 + patch * 10)
#if defined(_MSC_VER)
#define DW_MSVC
#elif defined(__clang__)
#define DW_CLANG __VERSION(__clang_major__, __clang_minor__, __clang_patchlevel__)
#if __has_include(<__config>)
#define DW_LIBCPP _LIBCPP_VERSION
#elif __has_include(<bits/c++config.h>)
#define DW_LIBSTDCPP __VERSION(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#else
#error This standard library is not recognised. Clang is only supported with libc++ or libstdc++.
#endif
#elif defined(__GNUC__)
#define DW_GCC __VERSION(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#define DW_LIBSTDCPP DW_GCC
#else
#error This compiler is not recognised. Aborting.
#endif
// clang-format on

// Define platform/build dependent visibility macro helpers.
#if defined(DW_DLL)
#if DW_PLATFORM == DW_WIN32
#define DW_HELPER_API_EXPORT __declspec(dllexport)
#define DW_HELPER_API_IMPORT __declspec(dllimport)
#else
#define DW_HELPER_API_EXPORT
#define DW_HELPER_API_IMPORT
#endif
#ifdef DW_BUILD
#define DW_API DW_HELPER_API_EXPORT
#else
#define DW_API DW_HELPER_API_IMPORT
#endif
#else
#define DW_API
#endif

// Define deprecated marker.
#ifdef DEPRECATED
#undef DEPRECATED
#endif
#if defined(DW_GCC) || defined(DW_CLANG)
#define DEPRECATED __attribute__((deprecated))
#elif defined(DW_MSVC)
#define DEPRECATED __declspec(deprecated)
#else
#pragma message("WARNING: DEPRECATED is not implemented for this compiler")
#define DEPRECATED
#endif

// Force inline and alignment - reuse Bullet's macros
#define DW_INLINE SIMD_FORCE_INLINE
#define DW_ALIGNED16(X) ATTRIBUTE_ALIGNED16(X)
#define DW_ALIGNED64(X) ATTRIBUTE_ALIGNED64(X)
#define DW_ALIGNED128(X) ATTRIBUTE_ALIGNED128(X)
