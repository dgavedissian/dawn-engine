/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "core/math/Vec2i.h"

#if defined(DW_MSVC)
#pragma warning(push)
#pragma warning(disable : 4127 4100)
#endif

#define TINYFORMAT_USE_VARIADIC_TEMPLATES
#include <tinyformat.h>

#if defined(DW_MSVC)
#pragma warning(pop)
#endif

namespace dw {
namespace str {

//// String

template <typename... Args> String format(const String& format, const Args&... args) {
    return tfm::format(format.c_str(), args...);
}

// Generates a unique name with a prefix - prefixes are grouped
DW_API String generateName(const String& prefix = "Unnamed");

// Pad a string to the right with spaces
DW_API String padString(const String& str, uint length);

// Split a string
DW_API Vector<String>& split(const String& s, char delim, Vector<String>& elems);

// Join a vector of strings
DW_API String join(const Vector<String>& vec, const String& delim);

DW_API String replace(String subject, const String& search, const String& replace);
}  // namespace str
}  // namespace dw
