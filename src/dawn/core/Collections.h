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

namespace dw {
template <class T, int N> using Array = std::array<T, N>;
template <class T> using Vector = std::vector<T>;
template <class T> using List = std::list<T>;
template <class K, class T> using Map = std::map<K, T>;
template <class K, class T> using HashMap = std::unordered_map<K, T>;
template <class T1, class T2> using Pair = std::pair<T1, T2>;
template <class... T> using Tuple = std::tuple<T...>;

template <class T1, class T2> inline Pair<T1, T2> makePair(T1&& a, T2&& b) {
    return std::pair<T1, T2>(std::forward<T1>(a), std::forward<T2>(b));
}

template <class... T> inline Tuple<T...> makeTuple(T&&... args) {
    return std::tuple<T...>(std::forward<T>(args)...);
}
}