/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
//#include "Camera.h"
#include "core/scene/LargePosition.h"

namespace dw {

const LargePosition LargePosition::origin;

LargePosition::LargePosition() : x{0.0}, y{0.0}, z{0.0} {
}

LargePosition::LargePosition(double x, double y, double z) : x{x}, y{y}, z{z} {
}

LargePosition::LargePosition(const Vec3& vector)
    : x(static_cast<double>(vector.x)),
      y(static_cast<double>(vector.y)),
      z(static_cast<double>(vector.z)) {
}

LargePosition::LargePosition(const LargePosition& other) : x{other.x}, y{other.y}, z{other.z} {
}

Vec3 LargePosition::getRelativeTo(const LargePosition& point) const {
    LargePosition delta = *this - point;
    return Vec3(static_cast<float>(delta.x), static_cast<float>(delta.y),
                static_cast<float>(delta.z));
}

Vec3 LargePosition::toCameraSpace(const LargePosition& camera_position) const {
    return getRelativeTo(camera_position);
}

LargePosition LargePosition::fromCameraSpace(const LargePosition& camera_position,
                                             const Vec3& point) {
    return camera_position + point;
}

bool LargePosition::operator==(const LargePosition& other) const {
    return floatEq(x, other.x) && floatEq(y, other.y) && floatEq(z, other.z);
}

bool LargePosition::operator!=(const LargePosition& other) const {
    return !(*this == other);
}

LargePosition& LargePosition::operator=(const LargePosition& other) {
    x = other.x;
    y = other.y;
    z = other.z;
    return *this;
}

LargePosition& LargePosition::operator=(const Vec3& other) {
    *this = LargePosition(other);
    return *this;
}

LargePosition& LargePosition::operator+=(const LargePosition& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

LargePosition& LargePosition::operator+=(const Vec3& other) {
    return *this += LargePosition(other);
}

LargePosition& LargePosition::operator-=(const LargePosition& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

LargePosition& LargePosition::operator-=(const Vec3& other) {
    return *this -= LargePosition(other);
}

LargePosition& LargePosition::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

LargePosition& LargePosition::operator/=(float scalar) {
    return *this *= (1.0f / scalar);
}

LargePosition LargePosition::operator-() const {
    return LargePosition(-x, -y, -z);
}

LargePosition LargePosition::operator+(const LargePosition& other) const {
    return LargePosition(x + other.x, y + other.y, z + other.z);
}

LargePosition LargePosition::operator+(const Vec3& other) const {
    return *this + LargePosition(other);
}

LargePosition LargePosition::operator-(const LargePosition& other) const {
    return LargePosition(x - other.x, y - other.y, z - other.z);
}

LargePosition LargePosition::operator-(const Vec3& other) const {
    return *this - LargePosition(other);
}

LargePosition LargePosition::operator*(float scalar) const {
    return LargePosition(x * scalar, y * scalar, z * scalar);
}

LargePosition LargePosition::operator/(float scalar) const {
    return *this * (1.0f / scalar);
}
}  // namespace dw
