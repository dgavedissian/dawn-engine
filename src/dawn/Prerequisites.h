/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "Platform.h"

// Mark this header as a system header
#if defined(DW_GCC) || defined(DW_CLANG)
#pragma GCC system_header
#elif defined(DW_MSVC)
#pragma warning(push, 0)
#endif

//
// Standard library
//

#include <cassert>
#include <cstdint>
#include <ctime>

#include <exception>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <random>
#include <memory>
#include <functional>
#include <thread>
#include <limits>
#include <typeinfo>
#include <chrono>
#include <mutex>

//
// External Libraries
//

// bgfx
#include <bgfx/bgfx.h>

// glfw
#include <GLFW/glfw3.h>

// Bullet
#include <btBulletDynamicsCommon.h>

// MathGeoLib
#define MATH_ENABLE_STL_SUPPORT
#define MATH_ENABLE_UNCOMMON_OPERATIONS
#define MATH_BULLET_INTEROP
#define LOGGING_SUPPORT_DISABLED
#include <MathGeoLib.h>
#ifdef LOG
#undef LOG
#endif

//
// Bundled Libraries
//

// imgui
#include "imgui.h"

// FastDelegate
#include "FastDelegate.h"

// Re-enable warnings
#if defined(DW_MSVC)
#pragma warning(pop)
#endif

// Disable "warning C4251: '...' needs to have dll-interface to be used by clients of class '...'"
#if defined(DW_MSVC)
#pragma warning(disable : 4251)
#endif
