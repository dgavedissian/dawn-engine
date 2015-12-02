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
using uint = unsigned int;
using int8 = int8_t;
using uint8 = uint8_t;
using int16 = int16_t;
using uint16 = uint16_t;
using int32 = int32_t;
using uint32 = uint32_t;
using int64 = int64_t;
using uint64 = uint64_t;

// String
using String = std::string;
using StringStream = std::stringstream;

// Smart pointers
template <class T> using SharedPtr = std::shared_ptr<T>;
template <class T> using WeakPtr = std::weak_ptr<T>;
template <class T> using UniquePtr = std::unique_ptr<T>;
template <class T> using EnableSharedFromThis = std::enable_shared_from_this<T>;
template <class T, class... Args> inline SharedPtr<T> MakeShared(Args&&... args)
{
#ifdef DW_MSVC
    // MSVC's implementation of make_shared seems to crash in seemingly random scenarios
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
#else
    return std::make_shared<T>(std::forward<Args>(args)...);
#endif
}
template <class T, class... Args> inline UniquePtr<T> MakeUnique(Args&&... args)
{
    // C++14 feature
#ifdef DW_MSVC
    return std::make_unique<T>(std::forward<Args>(args)...);
#else
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
#endif
}
template <class T1, class T2> inline SharedPtr<T1> StaticPointerCast(const SharedPtr<T2>& other)
{
    return std::static_pointer_cast<T1, T2>(other);
}
template <class T1, class T2> inline SharedPtr<T1> DynamicPointerCast(const SharedPtr<T2>& other)
{
    return std::dynamic_pointer_cast<T1, T2>(other);
}

// Pair and tuples
template <class T1, class T2> using Pair = std::pair<T1, T2>;
template <class... T> using Tuple = std::tuple<T...>;
template <class T1, class T2> inline Pair<T1, T2> MakePair(T1&& a, T2&& b)
{
    return std::pair<T1, T2>(std::forward<T1>(a), std::forward<T2>(b));
}
template <class... T> inline Tuple<T...> MakeTuple(T&&... args)
{
    return std::tuple<T...>(std::forward<T>(args)...);
}

// Files
// TODO: Deprecated, use file class
using std::ifstream;
using std::ofstream;
using std::fstream;

// Colour Value
// TODO: Implement this ourselves instead of using ogres
using Colour = Ogre::ColourValue;

// Vectors
using Vec2 = math::float2;
using Vec3 = math::float3;
using Vec4 = math::float4;

// Matrices
using Mat2 = math::float2x2;
using Mat3 = math::float3x3;
using Mat4 = math::float4x4;

// Quaternion
using Quat = math::Quat;

// Plane
using Plane = math::Plane;

// Containers
template <class T> using Vector = std::vector<T>;
template <class T> using List = std::list<T>;
template <class K, class T> using Map = std::map<K, T>;

NAMESPACE_END

// Safe delete macros
#define SAFE_DELETE(x) if ((x)) { delete (x); x = nullptr; }

// Error macros
#define ERROR_WARN(x) LOGWARN << (x)
#define ERROR_FATAL(x) { LOGERR << (x); throw std::runtime_error(x); }

// Math library
#include "Math/MathDefs.h"
#include "Math/Noise.h"
#include "Math/Vec2i.h"
#include "Math/Vec3i.h"
#include "Math/Vec4i.h"
#include "Math/Rect.h"
// TODO: Random number generators

// Data structures
#include "Core/FixedMemoryPool.h"

// Platform library
// Thread
// File

// Position
#include "Scene/Position.h"

// Core
#include "Core/Singleton.h"
#include "Core/Log.h"
#include "Core/Config.h"
#include "Core/Utils.h"

// Event System
#include "EventSystem.h"
