/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "Vec3i.h"

NAMESPACE_BEGIN

Vec3i::Vec3i() : x(0), y(0), z(0)
{
}

Vec3i::Vec3i(int _x, int _y, int _z) : x(_x), y(_y), z(_z)
{
}

Vec3i::Vec3i(const Vec3& v) : x((int)v.x), y((int)v.y), z((int)v.z)
{
}

Vec3i& Vec3i::operator=(const Vec3i& other)
{
    x = other.x;
    y = other.y;
    z = other.z;
    return *this;
}

Vec3i& Vec3i::operator+=(const Vec3i& other)
{
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Vec3i& Vec3i::operator-=(const Vec3i& other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Vec3i& Vec3i::operator*=(int scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vec3i& Vec3i::operator/=(int scalar)
{
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
}

const Vec3i Vec3i::operator-() const
{
    return Vec3i(-x, -y, -z);
}

const Vec3i Vec3i::operator+(const Vec3i& other) const
{
    return Vec3i(x + other.x, y + other.y, z + other.z);
}

const Vec3i Vec3i::operator-(const Vec3i& other) const
{
    return Vec3i(x - other.x, y - other.y, z - other.z);
}

const Vec3i Vec3i::operator*(int scalar) const
{
    return Vec3i(x * scalar, y * scalar, z * scalar);
}

const Vec3i Vec3i::operator/(int scalar) const
{
    return Vec3i(x / scalar, y / scalar, z / scalar);
}

NAMESPACE_END
