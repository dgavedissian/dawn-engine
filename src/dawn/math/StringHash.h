/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

namespace dw {
using StringHash = u32;
using StringHash64 = u64;

// FNV1a c++11 constexpr compile time hash functions, 32 and 64 bit
// str should be a null terminated string literal, value should be left out
// e.g StringHash("example")
// code license: public domain or equivalent
// post: https://notes.underscorediscovery.com/constexpr-fnv1a/

constexpr StringHash val_32_const = 0x811c9dc5;
constexpr StringHash prime_32_const = 0x1000193;
constexpr StringHash64 val_64_const = 0xcbf29ce484222325;
constexpr StringHash64 prime_64_const = 0x100000001b3;

inline constexpr StringHash Hash(const char* const str,
                                 const StringHash value = val_32_const) noexcept {
    return (str[0] == '\0') ? value : Hash(&str[1], (value ^ StringHash(str[0])) * prime_32_const);
}

inline constexpr StringHash Hash(const String& str) noexcept {
    return Hash(str.c_str());
}

inline constexpr StringHash64 Hash64(const char* const str,
                                     const StringHash64 value = val_64_const) noexcept {
    return (str[0] == '\0') ? value
                            : Hash64(&str[1], (value ^ StringHash64(str[0])) * prime_64_const);
}

inline constexpr StringHash64 Hash64(const String& str) noexcept {
    return Hash(str.c_str());
}
}  // namespace dw
