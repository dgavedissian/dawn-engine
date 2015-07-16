/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

// Mark this header as a system header
#if defined(__GNUC__) || defined(__clang__)
#   pragma GCC system_header
#elif defined(_MSC_VER)
#   pragma warning(push, 0)
#endif

// SSE intrinsics
#if defined (__SSE4_2__) || defined(__SSE4_1__)
#   include <smmintrin.h>
#   define DW_USE_SSE
#endif

//
// Standard library
//

#include <cassert>
#include <cstdint>
#include <ctime>
#include <exception>
#include <List>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <tuple>
#include <random>
#include <memory>
#include <functional>
#include <thread>
#include <limits>
#include <typeinfo>


//
// External Libraries
//

// Bullet
#include <btBulletDynamicsCommon.h>

// irrKlang
#include <irrKlang.h>

// Lua
#include <lua.hpp>

// Ogre
#include <Ogre.h>
#include <ParticleUniversePlugin.h>

// MathGeoLib
#define MATH_ENABLE_STL_SUPPORT
#define MATH_ENABLE_UNCOMMON_OPERATIONS
#define MATH_OGRE_INTEROP
#define MATH_BULLET_INTEROP
#define MATH_IRRKLANG_INTEROP
#define LOGGING_SUPPORT_DISABLED
#include <MathGeoLib.h>
#ifdef LOG
#   undef LOG
#endif

// Rocket
#include <Rocket/Core.h>
#include <Rocket/Controls.h>
#include <Rocket/Controls/DataSource.h>
#include <Rocket/Debugger.h>

// SDL
#include <SDL.h>

// Undef things defined by Obj-C/Cocoa on OS X which conflicts with other libs
#ifdef None
#   undef None
#endif
#ifdef Nil
#   undef Nil
#endif


//
// Bundled Libraries
//

// Bullet/Ogre utility functions
#include "BtOgreGP.h"
#include "BtOgrePG.h"

// Lua C++ interface
#include "LuaBridge.h"

// imgui debugging UI
#include "imgui.h"

// FastDelegate
#include "FastDelegate.h"

// Re-enable warnings
#if defined(_MSC_VER)
#   pragma warning(pop)
#endif
