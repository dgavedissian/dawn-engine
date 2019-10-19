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

// FastDelegate
#include "FastDelegate.h"

namespace dw {
template <typename S> using Delegate = fastdelegate::FastDelegate<S>;

template <typename X, typename Y, typename R, typename... Args>
Delegate<R(Args...)> makeDelegate(Y* x, R (X::*func)(Args...)) {
    Delegate<R(Args...)> result;
    result = fastdelegate::MakeDelegate<X, Y, Args..., R>(x, func);
    return result;
}

template <typename X, typename Y, typename R, typename... Args>
Delegate<R(Args...)> makeDelegate(Y* x, R (X::*func)(Args...) const) {
    Delegate<R(Args...)> result;
    result = fastdelegate::MakeDelegate<X, Y, Args..., R>(x, func);
    return result;
}
}  // namespace dw

// Re-enable warnings
#if defined(DW_MSVC)
#pragma warning(pop)
#endif
