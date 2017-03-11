/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once
#include "math/Vec2i.h"

namespace dw {
namespace str {

//// Command Line

typedef Map<String, String> CommandLineArgs;
DW_API CommandLineArgs parseCommandArgs(int argc, char** argv);

//// String

// Generates a unique name with a prefix - prefixes are grouped
DW_API String generateName(const String& prefix = "Unnamed");

// Pad a string to the right with spaces
DW_API String padString(const String& str, uint length);

// Split a string
DW_API Vector<String>& split(const String& s, char delim, Vector<String>& elems);

// Join a vector of strings
DW_API String concat(const Vector<String>& vec, const String& delim);

DW_API String replace(String subject, const String& search, const String& replace);
}

//// Random Number Generator

DW_API DEPRECATED int randomInt(int min = 0, int max = INT_MAX);
DW_API DEPRECATED uint randomUnsigned(uint min = 0, uint max = UINT_MAX);
DW_API DEPRECATED float randomFloat(float min = 0.0f, float max = 1.0f);
DW_API DEPRECATED double randomDouble(double min = 0.0f, double max = 1.0f);
DW_API DEPRECATED Vec2 randomVec2(const Vec2& min = Vec2(-1.0f, -1.0f),
                                  const Vec2& max = Vec2(1.0f, 1.0f));
DW_API DEPRECATED Vec3 randomVec3(const Vec3& min = Vec3(-1.0f, -1.0f, -1.0f),
                                  const Vec3& max = Vec3(1.0f, 1.0f, 1.0f));
DW_API DEPRECATED Vec4 randomVec4(const Vec4& min = Vec4(-1.0f, -1.0f, -1.0f, -1.0f),
                                  const Vec4& max = Vec4(1.0f, 1.0f, 1.0f, 1.0f));
}
