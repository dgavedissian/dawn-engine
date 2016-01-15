/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

NAMESPACE_BEGIN

class Vec4i
{
public:
    int x, y, z, w;

    /// Constructors
    Vec4i();
    Vec4i(int _x, int _y, int _z, int _w);
    Vec4i(const Vec4& v);

    /// Operators
    Vec4i& operator=(const Vec4i& other);
    Vec4i& operator+=(const Vec4i& other);
    Vec4i& operator-=(const Vec4i& other);
    Vec4i& operator*=(int scalar);
    Vec4i& operator/=(int scalar);
    const Vec4i operator-() const;
    const Vec4i operator+(const Vec4i& other) const;
    const Vec4i operator-(const Vec4i& other) const;
    const Vec4i operator*(int scalar) const;
    const Vec4i operator/(int scalar) const;
};

NAMESPACE_END
