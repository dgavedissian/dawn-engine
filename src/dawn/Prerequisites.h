/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "Platform.h"

// Mark this header as a system header
#if defined(DW_GCC) || defined(DW_CLANG)
#   pragma GCC system_header
#elif defined(DW_MSVC)
#   pragma warning(push, 0)
#endif


//
// Standard library
//

#include <cassert>
#include <cstdint>
#include <ctime>

#include <exception>
#include <list>
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

// Undefine conflicting names define by stupid platform SDKs
#ifdef PlaySound
#   undef PlaySound
#endif
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
#if defined(DW_MSVC)
#   pragma warning(pop)
#endif
