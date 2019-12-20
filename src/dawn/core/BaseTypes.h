/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

// Mark this header as a system header
#if defined(DW_GCC) || defined(DW_CLANG)
#pragma GCC system_header
#elif defined(DW_MSVC)
#pragma warning(push, 0)
#endif

#include <cstdint>
#include <string>
#include <sstream>
#include <functional>
#include <memory>

// Re-enable warnings
#if defined(DW_MSVC)
#pragma warning(pop)
#endif

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
using usize = std::size_t;

// String
using String = std::string;
using StringStream = std::stringstream;

// Function
template <typename T> using Function = std::function<T>;

// Smart pointers
template <typename T> using SharedPtr = std::shared_ptr<T>;
template <typename T> using WeakPtr = std::weak_ptr<T>;
template <typename T, typename D = std::default_delete<T>> using UniquePtr = std::unique_ptr<T, D>;
template <typename T> using EnableSharedFromThis = std::enable_shared_from_this<T>;
template <typename T, typename... Args> SharedPtr<T> makeShared(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}
template <typename T, typename... Args> UniquePtr<T> makeUnique(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}
template <typename T1, typename T2> SharedPtr<T1> staticPointerCast(const SharedPtr<T2>& other) {
    return std::static_pointer_cast<T1, T2>(other);
}
template <typename T1, typename T2> SharedPtr<T1> dynamicPointerCast(const SharedPtr<T2>& other) {
    return std::dynamic_pointer_cast<T1, T2>(other);
}
}  // namespace dw
