/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "core/math/Vec2i.h"

namespace dw {
namespace str {

//// Command Line

using CommandLineArgs = Map<String, String>;
DW_API CommandLineArgs parseCommandArgs(int argc, char** argv);

//// String

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
