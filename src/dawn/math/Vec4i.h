/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

namespace dw {

class DW_API Vec4i {
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
}
