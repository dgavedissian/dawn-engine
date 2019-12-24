/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

// Macro machinery to enable macro overloading. Source: https://stackoverflow.com/a/24028231

#define GLUE(x, y) x y

#define RETURN_ARG_COUNT(_1, _2, _3, _4, _5, _6, count, ...) count
#define EXPAND_ARGS(args) RETURN_ARG_COUNT args
#define COUNT_ARGS_MAX6(...) EXPAND_ARGS((__VA_ARGS__, 6, 5, 4, 3, 2, 1, 0))

#define OVERLOAD_MACRO2(name, count) name##count
#define OVERLOAD_MACRO1(name, count) OVERLOAD_MACRO2(name, count)
#define OVERLOAD_MACRO(name, count) OVERLOAD_MACRO1(name, count)

#define CALL_OVERLOAD(name, ...) \
    GLUE(OVERLOAD_MACRO(name, COUNT_ARGS_MAX6(__VA_ARGS__)), (__VA_ARGS__))
