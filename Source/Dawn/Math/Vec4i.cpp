/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#include "Common.h"
#include "Vec4i.h"

NAMESPACE_BEGIN

Vec4i::Vec4i() : x(0), y(0), z(0), w(0)
{
}

Vec4i::Vec4i(int _x, int _y, int _z, int _w) : x(_x), y(_y), z(_z), w(_w)
{
}

Vec4i::Vec4i(const Vec4& v) : x((int)v.x), y((int)v.y), z((int)v.z), w((int)v.w)
{
}

Vec4i& Vec4i::operator=(const Vec4i& other)
{
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;
    return *this;
}

Vec4i& Vec4i::operator+=(const Vec4i& other)
{
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;
    return *this;
}

Vec4i& Vec4i::operator-=(const Vec4i& other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;
    return *this;
}

Vec4i& Vec4i::operator*=(int scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    w *= scalar;
    return *this;
}

Vec4i& Vec4i::operator/=(int scalar)
{
    x /= scalar;
    y /= scalar;
    z /= scalar;
    w /= scalar;
    return *this;
}

const Vec4i Vec4i::operator-() const
{
    return Vec4i(-x, -y, -z, -w);
}

const Vec4i Vec4i::operator+(const Vec4i& other) const
{
    return Vec4i(x + other.x, y + other.y, z + other.z, w + other.w);
}

const Vec4i Vec4i::operator-(const Vec4i& other) const
{
    return Vec4i(x - other.x, y - other.y, z - other.z, w - other.w);
}

const Vec4i Vec4i::operator*(int scalar) const
{
    return Vec4i(x * scalar, y * scalar, z * scalar, w * scalar);
}

const Vec4i Vec4i::operator/(int scalar) const
{
    return Vec4i(x / scalar, y / scalar, z / scalar, w / scalar);
}

NAMESPACE_END
