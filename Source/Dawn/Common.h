/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

// Include library prerequisites
#include "Prerequisites.h"

// Determine platform
#include "Platform.h"

// Module macros
#define NAMESPACE_BEGIN namespace dw {
#define NAMESPACE_END }

// Version Information
#define DW_VERSION_MAJOR 0
#define DW_VERSION_MINOR 1
#define DW_VERSION_PATCH 0
#define DW_VERSION_STR "0.1.0"

NAMESPACE_BEGIN

// Move luabridge to dw namespace
using namespace luabridge;

// Primitive types
typedef unsigned int uint;
typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;

// String
typedef std::string string;

// Smart pointers
using std::shared_ptr;
using std::weak_ptr;
using std::unique_ptr;
using std::make_shared;
using std::enable_shared_from_this;
using std::dynamic_pointer_cast;
using std::static_pointer_cast;

// Pair and tuples
using std::pair;
using std::tuple;
using std::make_pair;
using std::make_tuple;

// Container types
using std::map;
using std::vector;

// Files
using std::ifstream;
using std::ofstream;
using std::fstream;

// Colour Value
// TODO: Write this class instead of stealing Ogre's one
typedef Ogre::ColourValue Colour;

// Vectors
typedef math::float2 Vec2;
typedef math::float3 Vec3;
typedef math::float4 Vec4;

// Matrices
typedef math::float2x2 Mat2;
typedef math::float3x3 Mat3;
typedef math::float4x4 Mat4;

// Quaternion
typedef math::Quat Quat;

// Plane
typedef math::Plane Plane;

NAMESPACE_END

// Safe delete macros
#define SAFE_DELETE(x) if ((x)) { delete (x); x = nullptr; }

// Error macros
#define ERROR_WARN(x) LOGWARN << (x)
#define ERROR_FATAL(x) { LOGERR << (x); throw std::runtime_error(x); }

// Math library
#include "Math/Vec2i.h"
#include "Math/Vec3i.h"
#include "Math/Vec4i.h"
#include "Math/Rect.h"
#include "Math/MathDefs.h"
// Random number generators

// Data structures
// List
// Vector
// Map
// String
#include "Core/FixedMemoryPool.h"

// Platform library
// Thread
// File

// Position
#include "Scene/Position.h"

// Core Headers
#include "Core/Singleton.h"
#include "Core/Log.h"
#include "Core/Config.h"
#include "Core/Utils.h"

// Event System
#include "EventSystem.h"
