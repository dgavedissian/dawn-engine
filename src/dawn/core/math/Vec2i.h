/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once
#include "Defs.h"

namespace dw {

class DW_API Vec2i {
public:
    int x, y;

    /// Constructors
    Vec2i();
    Vec2i(int _x, int _y);
    explicit Vec2i(const Vec2& v);
    Vec2i(const Vec2i& v) = default;

    /// Operators
    Vec2i& operator=(const Vec2i& other);
    Vec2i& operator+=(const Vec2i& other);
    Vec2i& operator-=(const Vec2i& other);
    Vec2i& operator*=(int scalar);
    Vec2i& operator/=(int scalar);
    Vec2i& operator*=(float scalar);
    Vec2i& operator/=(float scalar);
    Vec2i& operator*=(const Vec2& other);
    Vec2i& operator/=(const Vec2& other);
    Vec2i operator-() const;
    Vec2i operator+(const Vec2i& other) const;
    Vec2i operator-(const Vec2i& other) const;
    Vec2i operator*(int scalar) const;
    Vec2i operator/(int scalar) const;
    Vec2i operator*(float scalar) const;
    Vec2i operator/(float scalar) const;
    Vec2i operator*(const Vec2& other) const;
    Vec2i operator/(const Vec2& other) const;
};
}  // namespace dw
