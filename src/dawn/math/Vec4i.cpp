/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "math/Vec4i.h"

namespace dw {

Vec4i::Vec4i() : x(0), y(0), z(0), w(0) {
}

Vec4i::Vec4i(int _x, int _y, int _z, int _w) : x(_x), y(_y), z(_z), w(_w) {
}

Vec4i::Vec4i(const Vec4& v)
    : x(static_cast<int>(v.x)),
      y(static_cast<int>(v.y)),
      z(static_cast<int>(v.z)),
      w(static_cast<int>(v.w)) {
}

Vec4i& Vec4i::operator=(const Vec4i& other) {
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;
    return *this;
}

Vec4i& Vec4i::operator+=(const Vec4i& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;
    return *this;
}

Vec4i& Vec4i::operator-=(const Vec4i& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;
    return *this;
}

Vec4i& Vec4i::operator*=(int scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    w *= scalar;
    return *this;
}

Vec4i& Vec4i::operator/=(int scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    w /= scalar;
    return *this;
}

Vec4i Vec4i::operator-() const {
    return Vec4i(-x, -y, -z, -w);
}

Vec4i Vec4i::operator+(const Vec4i& other) const {
    return Vec4i(x + other.x, y + other.y, z + other.z, w + other.w);
}

Vec4i Vec4i::operator-(const Vec4i& other) const {
    return Vec4i(x - other.x, y - other.y, z - other.z, w - other.w);
}

Vec4i Vec4i::operator*(int scalar) const {
    return Vec4i(x * scalar, y * scalar, z * scalar, w * scalar);
}

Vec4i Vec4i::operator/(int scalar) const {
    return Vec4i(x / scalar, y / scalar, z / scalar, w / scalar);
}
}  // namespace dw
