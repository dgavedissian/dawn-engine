/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

NAMESPACE_BEGIN

//// Command Line

typedef std::map<string, string> CommandLineArgs;
DW_API CommandLineArgs ParseCommandArgs(int argc, char** argv);


//// String

// Generates a unique name with a prefix - prefixes are grouped
DW_API string GenerateName(const string& prefix = "Unnamed");

// Pad a string to the right with spaces
DW_API string PadString(const string& str, uint length);

// Split a string
DW_API std::vector<string>& Split(const string& s, char delim, std::vector<string>& elems);

// Join a vector of strings
DW_API string Concat(const vector<string>& vec, const string& delim);


//// Random Number Generator

DW_API DEPRECATED int RandomInt(int min = 0, int max = INT_MAX);
DW_API DEPRECATED uint RandomUint(uint min = 0, uint max = UINT_MAX);
DW_API DEPRECATED float RandomFloat(float min = 0.0f, float max = 1.0f);
DW_API DEPRECATED double RandomDouble(double min = 0.0f, double max = 1.0f);
DW_API DEPRECATED Vec2 RandomVec2(const Vec2& min = Vec2(-1.0f, -1.0f), const Vec2& max = Vec2(1.0f, 1.0f));
DW_API DEPRECATED Vec3 RandomVec3(const Vec3& min = Vec3(-1.0f, -1.0f, -1.0f),
                                  const Vec3& max = Vec3(1.0f, 1.0f, 1.0f));
DW_API DEPRECATED Vec4 RandomVec4(const Vec4& min = Vec4(-1.0f, -1.0f, -1.0f, -1.0f),
                                  const Vec4& max = Vec4(1.0f, 1.0f, 1.0f, 1.0f));


//// Time

namespace time
{
    typedef double TimePoint;
    typedef double Duration;

    // Get the current time - doesn't really matter when the timer starts as long as difference
    // between time points can be used to measure time accurately
    DW_API TimePoint Now();

    // Get the time elapsed since a certain time point
    DW_API Duration GetElapsed(TimePoint tp);

    // Format time
    DW_API string Format(time_t time, const string& formatString);
}

NAMESPACE_END
