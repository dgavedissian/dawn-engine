/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

NAMESPACE_BEGIN

// Perlin Noise class
#define SAMPLE_SIZE 1024
class DW_API PerlinNoise
{
public:
    PerlinNoise(int octaves, float freq, float amp, int seed);
    float noise(float x);
    float noise(float x, float y);
    float noise(float x, float y, float z);

private:
    float perlinNoise1(float arg);
    float perlinNoise2(float vec[2]);
    float perlinNoise3(float vec[3]);

    void init();
    float noise1(float arg);
    float noise2(float vec[2]);
    float noise3(float vec[3]);
    void normalise2(float v[2]);
    void normalise3(float v[3]);

    int mOctaves;
    float mFrequency;
    float mAmplitude;
    int mSeed;

    int p[SAMPLE_SIZE + SAMPLE_SIZE + 2];
    float g3[SAMPLE_SIZE + SAMPLE_SIZE + 2][3];
    float g2[SAMPLE_SIZE + SAMPLE_SIZE + 2][2];
    float g1[SAMPLE_SIZE + SAMPLE_SIZE + 2];
    bool mStart;
};

template <class T>
bool FloatEq(T a, T b, T epsilon = std::numeric_limits<T>::epsilon())
{
    return math::Abs(a - b) <= epsilon;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Command Line
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef std::map<string, string> CommandLineArgs;

CommandLineArgs ParseCommandArgs(int argc, char** argv);

////////////////////////////////////////////////////////////////////////////////////////////////////
// Geometry
////////////////////////////////////////////////////////////////////////////////////////////////////

// Create a sphere mesh with a given name, radius, number of rings and number of segments
DW_API Ogre::MeshPtr CreateSphere(const string& name, float radius, int rings, int segments,
                                  bool normals, bool texCoords);

// Fill up a fresh copy of VertexData and IndexData with a sphere's coords given the number of rings
// and the number of segments
DW_API void CreateSphere(Ogre::VertexData* vertexData, Ogre::IndexData* indexData, float radius,
                         int rings, int segments, bool normals, bool texCoords);

// Create a cone mesh with a given name, radius and number of vertices in the base. The created cone
// will have its head at 0, 0, 0, and will 'expand to' positive y
DW_API Ogre::MeshPtr CreateCone(const string& name, float radius, float height, int verticesInBase);

// Fill up a fresh copy of VertexData and IndexData with a cone's coords given the radius and number
// of vertices in the base
DW_API void CreateCone(Ogre::VertexData* vertexData, Ogre::IndexData* indexData, float radius,
                       float height, int verticesInBase);

// Fill up a fresh copy of VertexData with a normalized quad
DW_API void CreateQuad(Ogre::VertexData* vertexData);


////////////////////////////////////////////////////////////////////////////////////////////////////
// Units
////////////////////////////////////////////////////////////////////////////////////////////////////

enum DistUnits
{
    UNIT_M,
    UNIT_KM,
    UNIT_MI,
    UNIT_AU,
    UNIT_LY,
    UNIT_PC
};

enum MassUnits
{
    UNIT_G,
    UNIT_KG,
    UNIT_T,
    UNIT_SM    // Solar Mass
};

enum TimeUnits
{
    UNIT_SECOND,
    UNIT_MINUTE,
    UNIT_HOUR,
    UNIT_DAY,
    UNIT_YEAR,
};

template <class T>
T ConvUnit(T input, DistUnits outUnit, DistUnits inUnit = UNIT_M)
{
    // Unit to value lookup function
    auto lookup = [](DistUnits unit) -> T
    {
        switch (unit)
        {
        case UNIT_M:
            return (T)1.0;

        case UNIT_KM:
            return (T)1e3;    // 1 km = 10^3 m

        case UNIT_MI:
            return (T)1609.344;    // 1 mile = 1609.344 m

        case UNIT_AU:
            return (T)149597870700.0;    // 1 astronomical unit = 1.5x10^11 m

        case UNIT_LY:
            return (T)9.46073047e15;    // 1 light-year = 9.46x10^15 m

        case UNIT_PC:
            return (T)3.08567758e16;    // 1 parsec = 3.057x10^16 m

        default:
            assert(false);
            return (T)0.0;
        }
    };

    // Special cases: light-years to parsecs; parsecs to light-years.
    if (inUnit == UNIT_LY && outUnit == UNIT_PC)
    {
        return input * (T)0.306594845;    // 1 parsec = 0.307 light-years
    }

    if (inUnit == UNIT_PC && outUnit == UNIT_LY)
    {
        return input * (T)3.26163344332;    // 3.26 light years = 1 parsec
    }

    // Lookup the units and convert
    return (input / lookup(inUnit)) * lookup(outUnit);
}

template <class T>
T ConvUnit(T input, MassUnits outUnit, MassUnits inUnit = UNIT_KG)
{
    // Unit to value lookup function
    auto lookup = [](MassUnits unit) -> T
    {
        switch (unit)
        {
        case UNIT_G:
            return (T)0.001;    // 1 g = 0.001 kg

        case UNIT_KG:
            return (T)1.0;

        case UNIT_T:
            return (T)1000.0;    // 1 tonne = 1000 kg

        case UNIT_SM:
            return (T)1.9891e30;    // 1 solar mass = 1.99x10^30 kg

        default:
            assert(false);
            return (T)0.0;
        }
    };

    // Lookup the units and convert
    return (input / lookup(inUnit)) * lookup(outUnit);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Math
////////////////////////////////////////////////////////////////////////////////////////////////////

// Lerp taking into account the rate of time - in 1 second, value will be x between a and b.
template <class T> T Lerp(const T& a, const T& b, float x, float dt)
{
    return math::Lerp(a, b, 1.0f - pow(1.0f - x, dt));
}

// Implementation of Lerp for Position objects
template <> Position Lerp(const Position& a, const Position& b, float x, float dt);

// Implementation of Lerp for Quaternion objects (uses Slerp)
template <> Quat Lerp(const Quat& a, const Quat& b, float x, float dt);

// Takes one step towards a value by a certain amount
template <class T> T Step(const T& x, const T& step, const T& target)
{
    if (target > x)
        return math::Min(target, x + step);
    if (target < x)
        return math::Max(target, x - step);
    return x;
}

// Floating-point modulo
// The result (the remainder) has same sign as the divisor
// Similar to matlab's mod(); Not similar to fmod() - mod(-3,4) = 1
// fmod(-3,4)
// = -3
template <typename T> T Mod(T x, T y)
{
    if (y == 0.0f)
        return x;

    double m = x - y * floor(x / y);

    if (y > 0)    // modulo range: [0..y)
    {
        if (m >= y)    // mod(-1e-16, 360.0): m = 360.0
            return 0;

        if (m < 0)
        {
            if (FloatEq<T>(y + m, y))
                return 0;
            else
                return y + m;    // mod(106.81415022205296, _TWO_PI ): m = -1.421e-14
        }
    }
    else    // modulo range: (y..0]
    {
        if (m <= y)    // mod(1e-16, -360.0): m = -360.0
            return 0;

        if (m > 0)
        {
            if (FloatEq<T>(y + m, y))
                return 0;
            else
                return y + m;    // mod(-106.81415022205296, -_TWO_PI): m = 1.421e-14
        }
    }

    return m;
}

// Limits a value to a certain range by "wrapping" the point back to the beginning much like the
// modulus operation
template <class T>
T Wrap(const T& x, const T& a, const T& b)
{
    return Mod(x - a, b - a) + a;
}

// Estimate the position to fire at to hit a moving target given a position, speed, target position
// and target velocity
DW_API Position EstimateHitPosition(const Position& position, float speed,
                                      const Position& targetPosition, const Vec3& targetVelocity);


////////////////////////////////////////////////////////////////////////////////////////////////////
// String
////////////////////////////////////////////////////////////////////////////////////////////////////

// Generates a unique name with a prefix - prefixes are grouped
DW_API string GenerateName(const string& prefix = "Unnamed");

// Pad a string to the right with spaces
DW_API string PadString(const string& str, uint length);

// Split a string
DW_API std::vector<string>& Split(const string& s, char delim, std::vector<string>& elems);

// Join a vector of strings
DW_API string Concat(const vector<string>& vec, const string& delim);


////////////////////////////////////////////////////////////////////////////////////////////////////
// Random Number Generator
////////////////////////////////////////////////////////////////////////////////////////////////////

DW_API int randomInt(int min = 0, int max = INT_MAX);
DW_API uint randomUnsigned(uint min = 0, uint max = UINT_MAX);
DW_API float randomFloat(float min = 0.0f, float max = 1.0f);
DW_API double randomDouble(double min = 0.0f, double max = 1.0f);
DW_API Vec2 randomVec2(const Vec2& min = Vec2(-1.0f, -1.0f), const Vec2& max = Vec2(1.0f, 1.0f));
DW_API Vec3 randomVec3(const Vec3& min = Vec3(-1.0f, -1.0f, -1.0f),
                       const Vec3& max = Vec3(1.0f, 1.0f, 1.0f));
DW_API Vec4 randomVec4(const Vec4& min = Vec4(-1.0f, -1.0f, -1.0f, -1.0f),
                       const Vec4& max = Vec4(1.0f, 1.0f, 1.0f, 1.0f));


////////////////////////////////////////////////////////////////////////////////////////////////////
// Time
////////////////////////////////////////////////////////////////////////////////////////////////////
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
