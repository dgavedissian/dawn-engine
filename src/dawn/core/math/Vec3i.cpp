/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "Vec3i.h"

namespace dw {

Vec3i::Vec3i() : x(0), y(0), z(0) {
}

Vec3i::Vec3i(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {
}

Vec3i::Vec3i(const Vec3& v)
    : x(static_cast<int>(v.x)), y(static_cast<int>(v.y)), z(static_cast<int>(v.z)) {
}

Vec3i& Vec3i::operator=(const Vec3i& other) {
    x = other.x;
    y = other.y;
    z = other.z;
    return *this;
}

Vec3i& Vec3i::operator+=(const Vec3i& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Vec3i& Vec3i::operator-=(const Vec3i& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Vec3i& Vec3i::operator*=(int scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vec3i& Vec3i::operator/=(int scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
}

Vec3i Vec3i::operator-() const {
    return Vec3i(-x, -y, -z);
}

Vec3i Vec3i::operator+(const Vec3i& other) const {
    return Vec3i(x + other.x, y + other.y, z + other.z);
}

Vec3i Vec3i::operator-(const Vec3i& other) const {
    return Vec3i(x - other.x, y - other.y, z - other.z);
}

Vec3i Vec3i::operator*(int scalar) const {
    return Vec3i(x * scalar, y * scalar, z * scalar);
}

Vec3i Vec3i::operator/(int scalar) const {
    return Vec3i(x / scalar, y / scalar, z / scalar);
}
}  // namespace dw
