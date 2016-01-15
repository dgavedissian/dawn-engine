/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

NAMESPACE_BEGIN

class Vec2i
{
public:
    int x, y;

    /// Constructors
    Vec2i();
    Vec2i(int _x, int _y);
    Vec2i(const Vec2& v);

    /// Operators
    Vec2i& operator=(const Vec2i& other);
    Vec2i& operator+=(const Vec2i& other);
    Vec2i& operator-=(const Vec2i& other);
    Vec2i& operator*=(int scalar);
    Vec2i& operator/=(int scalar);
    const Vec2i operator-() const;
    const Vec2i operator+(const Vec2i& other) const;
    const Vec2i operator-(const Vec2i& other) const;
    const Vec2i operator*(int scalar) const;
    const Vec2i operator/(int scalar) const;
};

NAMESPACE_END
