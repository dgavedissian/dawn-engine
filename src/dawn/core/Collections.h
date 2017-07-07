/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include <tuple>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>

#include <mpark/variant.hpp>

namespace dw {

// If using GCC, create a hash wrapper to work around std::hash<T> not working for enum
// classes.
#if defined(DW_GCC)
template <typename T, typename Enable = void> struct HashFunction {
    typedef typename std::hash<T>::argument_type argument_type;
    typedef typename std::hash<T>::result_type result_type;
    inline result_type operator()(argument_type const& s) const {
        std::hash<T> standard_hash;
        return standard_hash(s);
    }
};
template <typename E> struct HashFunction<E, std::enable_if_t<std::is_enum<E>::value>> {
    typedef E argument_type;
    typedef std::size_t result_type;
    inline result_type operator()(argument_type const& s) const {
        return static_cast<result_type>(s);
    }
};
#else
template <typename T> using HashFunction = std::hash<T>;
#endif

template <typename T, int N> using Array = std::array<T, N>;
template <typename T> using Vector = std::vector<T>;
template <typename T> using List = std::list<T>;
template <typename K, typename T> using Map = std::map<K, T>;
template <typename K, typename T> using HashMap = std::unordered_map<K, T, HashFunction<K>>;
template <typename T1, typename T2> using Pair = std::pair<T1, T2>;
template <typename... T> using Tuple = std::tuple<T...>;
template <typename... T> using Variant = mpark::variant<T...>;

template <typename T1, typename T2> inline Pair<T1, T2> makePair(T1&& a, T2&& b) {
    return std::pair<T1, T2>(std::forward<T1>(a), std::forward<T2>(b));
}

template <typename... T> inline Tuple<T...> makeTuple(T&&... args) {
    return std::tuple<T...>(std::forward<T>(args)...);
}
}  // namespace dw
