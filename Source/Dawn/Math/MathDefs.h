/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

#include <cmath>
#undef M_PI

NAMESPACE_BEGIN

static const float M_PI = 3.14159265358979323846264338327950288f;
static const float M_HALF_PI = M_PI * 0.5f;
static const float M_EPSILON = std::numeric_limits<float>::epsilon();
static const float M_LARGE_EPSILON = 0.00005f;
static const float M_LARGE_VALUE = 100000000.0f;
static const float M_INFINITY = HUGE_VALF;
static const float M_DEGTORAD = M_PI / 180.0f;
static const float M_DEGTORAD_OVER_2 = M_PI / 360.0f;
static const float M_RADTODEG = 1.0f / M_DEGTORAD;


//// Floating point functions

/// Raise the base to the given exponent
inline float Pow(float base, float exponent) { return powf(base, exponent); }

/// Raise the given base to the given integer exponent
inline float Pow(float base, int exponent) { return pow(base, exponent); }

/// Square root
inline float Sqrt(float value) { return sqrtf(value); }

/// Fast inverse square root. Originally from Quake III.
inline float FastSqrt(float value)
{
	float half = 0.5f * value;
	int i = *(int*)&value;

	// Initial guess y0
	i = 0x5f3759df - (i >> 1);

	// Perform a single newton iteration
	value = *(float*)&i;
	value = value * (1.5f - half * value * value);
	return value;
}

/// Linear interpolation between two float values.
inline float Lerp(float lhs, float rhs, float t) { return lhs * (1.0f - t) + rhs * t; }

/// Linear interpolation between two double values.
inline double Lerp(double lhs, double rhs, float t) { return lhs * (1.0f - t) + rhs * t; }

/// Linear interpolation between two float values taking into account the rate of time.
/// NOTE: in 1 second, value will be t% between the lhs and rhs.
inline float Lerp(float lhs, float rhs, float t, float dt)
{
	return Lerp(lhs, rhs, 1.0f - Pow(1.0f - t, dt));
}

/// Linear interpolation between two quaternion values taking into account the rate of time.
inline Quat Lerp(const Quat& a, const Quat& b, float t, float dt)
{
	return Quat::Slerp(a, b, 1.0f - Pow(1.0f - t, dt));
}

/// Return the smaller of two floats.
inline float Min(float lhs, float rhs) { return lhs < rhs ? lhs : rhs; }

/// Return the larger of two floats.
inline float Max(float lhs, float rhs) { return lhs > rhs ? lhs : rhs; }

/// Returns the greatest integer which is less than the input value
inline float Floor(float value) { return floorf(value); }

/// Returns the smallest integer which is greater than the input value
inline float Ceil(float value) { return ceilf(value); }

/// Return absolute value of a float.
inline float Abs(float value) { return value >= 0.0f ? value : -value; }

/// Return absolute value of a double.
inline double Abs(double value) { return value >= 0.0 ? value : -value; }

/// Return the sign of a float (-1, 0 or 1.)
inline float Sign(float value) { return value > 0.0f ? 1.0f : (value < 0.0f ? -1.0f : 0.0f); }

/// Fuzzy equals operator for floats.
inline bool FloatEq(float a, float b, float epsilon = M_EPSILON) { return Abs(a - b) <= epsilon; }

/// Fuzzy equals operator for doubles.
inline bool FloatEq(double a, double b, double epsilon = M_EPSILON) { return Abs(a - b) <= epsilon; }

/// Taken from Urho3D
/// Check whether a floating point value is NaN.
/// Use a workaround for GCC, see https://github.com/urho3d/Urho3D/issues/655
#ifndef __GNUC__
inline bool IsNaN(float value) { return value != value; }
#else
inline bool IsNaN(float value)
{
	unsigned u = *(unsigned*)(&value);
	return (u & 0x7fffffff) > 0x7f800000;
}
#endif

/// Clamp a float to a range.
inline float Clamp(float value, float min, float max)
{
	if (value < min)
		return min;
	else if (value > max)
		return max;
	else
		return value;
}

/// Smoothly damp between values.
inline float SmoothStep(float lhs, float rhs, float t)
{
	t = Clamp((t - lhs) / (rhs - lhs), 0.0f, 1.0f); // Saturate t
	return t * t * (3.0f - 2.0f * t);
}

/// Return sine of an angle in degrees.
inline float Sin(float angle) { return sinf(angle); }

/// Return cosine of an angle in degrees.
inline float Cos(float angle) { return cosf(angle); }

/// Return tangent of an angle in degrees.
inline float Tan(float angle) { return tanf(angle); }

/// Return arc sine in degrees.
inline float Asin(float x) { return asinf(Clamp(x, -1.0f, 1.0f)); }

/// Return arc cosine in degrees.
inline float Acos(float x) { return acosf(Clamp(x, -1.0f, 1.0f)); }

/// Return arc tangent in degrees.
inline float Atan(float x) { return atanf(x); }

/// Return arc tangent of y/x in degrees.
inline float Atan2(float y, float x) { return atan2f(y, x); }

/// Floating-point modulo
/// Note: The result (the remainder) has same sign as the divisor. Similar to matlab's mod(),
///	      rather than cmath's to fmod(). Eg: mod(-3,4) = 1, fmod(-3,4) = -3
inline float Mod(float x, float y)
{
	if (FloatEq(y, 0.0f))
		return x;

	float m = x - y * Floor(x / y);
	if (y > 0)    // modulo range: [0..y)
	{
		if (m >= y)    // mod(-1e-16, 360.0): m = 360.0
			return 0;

		if (m < 0)
		{
			if (FloatEq(y + m, y))
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
			if (FloatEq(y + m, y))
				return 0;
			else
				return y + m;    // mod(-106.81415022205296, -_TWO_PI): m = 1.421e-14
		}
	}

	return m;
}


//// Integer functions

/// Return the smaller of two integers.
inline int Min(int lhs, int rhs) { return lhs < rhs ? lhs : rhs; }

/// Return the larger of two integers.
inline int Max(int lhs, int rhs) { return lhs > rhs ? lhs : rhs; }

/// Return absolute value of an integer
inline int Abs(int value) { return value >= 0 ? value : -value; }

/// Clamp an integer to a range.
inline int Clamp(int value, int min, int max)
{
	if (value < min)
		return min;
	else if (value > max)
		return max;
	else
		return value;
}

/// Check whether an unsigned integer is a power of two.
inline bool IsPowerOfTwo(unsigned value)
{
	if (!value)
		return true;
	while (!(value & 1))
		value >>= 1;
	return value == 1;
}

/// Round up to next power of two.
inline unsigned NextPowerOfTwo(unsigned value)
{
	unsigned ret = 1;
	while (ret < value && ret < 0x80000000)
		ret <<= 1;
	return ret;
}

/// Count the number of set bits in a mask.
inline unsigned CountSetBits(unsigned value)
{
	// Brian Kernighan's method
	unsigned count = 0;
	for (count = 0; value; count++)
		value &= value - 1;
	return count;
}

/// Update a hash with the given 8-bit value using the SDBM algorithm.
inline unsigned SDBMHash(unsigned hash, unsigned char c)
{
	return c + (hash << 6) + (hash << 16) - hash;
}


//// Templated functions

/// Limits a value to a certain range by wrapping it past the constraints.
template <class T>
T Wrap(const T& value, const T& min, const T& max)
{
	return Mod(value - min, max - min) + min;
}

/// Takes one step from a value to a target with a given step. If the distance is less then the
/// step value, then it just returns the target value.
template <class T> T Step(const T& value, const T& step, const T& target)
{
	if (target > value)
		return Min(target, value + step);
	if (target < value)
		return Max(target, value - step);
	return value;
}

NAMESPACE_END
