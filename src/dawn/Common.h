/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

// Include library prerequisites
#include "Prerequisites.h"

// Version Information
#define DW_VERSION_MAJOR 0
#define DW_VERSION_MINOR 1
#define DW_VERSION_PATCH 0
#define DW_VERSION_STR "0.1.0 (Pre-alpha 1)"

namespace dw {

// Primitive types
using uint = unsigned int;
using i8 = std::int8_t;
using u8 = std::uint8_t;
using i16 = std::int16_t;
using u16 = std::uint16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;
using i64 = std::int64_t;
using u64 = std::uint64_t;
using byte = u8;
using intptr = std::intptr_t;
using uintptr = std::uintptr_t;

// String
using String = std::string;
using StringStream = std::stringstream;

// Function
template <typename T> using Function = std::function<T>;

// Smart pointers
template <typename T> using SharedPtr = std::shared_ptr<T>;
template <typename T> using WeakPtr = std::weak_ptr<T>;
template <typename T> using UniquePtr = std::unique_ptr<T>;
template <typename T> using EnableSharedFromThis = std::enable_shared_from_this<T>;
template <typename T, typename... Args> inline SharedPtr<T> makeShared(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}
template <typename T, typename... Args> inline UniquePtr<T> makeUnique(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}
template <typename T1, typename T2>
inline SharedPtr<T1> staticPointerCast(const SharedPtr<T2>& other) {
    return std::static_pointer_cast<T1, T2>(other);
}
template <typename T1, typename T2>
inline SharedPtr<T1> dynamicPointerCast(const SharedPtr<T2>& other) {
    return std::dynamic_pointer_cast<T1, T2>(other);
}
}  // namespace dw

// Error macros
#define ERROR_FATAL(x)               \
    {                                \
        log().error(x);              \
        throw std::runtime_error(x); \
    }

// Preprocessor library.
#include "core/Preprocessor.h"

// Common includes.
#include "core/Context.h"
#include "core/Exception.h"
#include "core/Object.h"
#include "core/Module.h"
#include "core/Log.h"
#include "core/EventSystem.h"
