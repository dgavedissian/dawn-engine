/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "renderer/SystemPosition.h"

namespace dw {
const SystemPosition SystemPosition::origin;

SystemPosition::SystemPosition() : x{0.0}, y{0.0}, z{0.0} {
}

SystemPosition::SystemPosition(double x, double y, double z) : x{x}, y{y}, z{z} {
}

SystemPosition::SystemPosition(const Vec3& vector)
    : x(static_cast<double>(vector.x)),
      y(static_cast<double>(vector.y)),
      z(static_cast<double>(vector.z)) {
}

SystemPosition::SystemPosition(const SystemPosition& other) : x{other.x}, y{other.y}, z{other.z} {
}

Vec3 SystemPosition::getRelativeTo(const SystemPosition& point) const {
    SystemPosition delta = *this - point;
    return Vec3(static_cast<float>(delta.x), static_cast<float>(delta.y),
                static_cast<float>(delta.z));
}

Vec3 SystemPosition::toCameraSpace(const SystemPosition& camera_position) const {
    return getRelativeTo(camera_position);
}

SystemPosition SystemPosition::fromCameraSpace(const SystemPosition& camera_position,
                                               const Vec3& point) {
    return camera_position + point;
}

bool SystemPosition::operator==(const SystemPosition& other) const {
    return floatEq(x, other.x) && floatEq(y, other.y) && floatEq(z, other.z);
}

bool SystemPosition::operator!=(const SystemPosition& other) const {
    return !(*this == other);
}

SystemPosition& SystemPosition::operator=(const SystemPosition& other) {
    x = other.x;
    y = other.y;
    z = other.z;
    return *this;
}

SystemPosition& SystemPosition::operator=(const Vec3& other) {
    *this = SystemPosition(other);
    return *this;
}

SystemPosition& SystemPosition::operator+=(const SystemPosition& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

SystemPosition& SystemPosition::operator+=(const Vec3& other) {
    return *this += SystemPosition(other);
}

SystemPosition& SystemPosition::operator-=(const SystemPosition& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

SystemPosition& SystemPosition::operator-=(const Vec3& other) {
    return *this -= SystemPosition(other);
}

SystemPosition& SystemPosition::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

SystemPosition& SystemPosition::operator/=(float scalar) {
    return *this *= (1.0f / scalar);
}

SystemPosition SystemPosition::operator-() const {
    return SystemPosition(-x, -y, -z);
}

SystemPosition SystemPosition::operator+(const SystemPosition& other) const {
    return SystemPosition(x + other.x, y + other.y, z + other.z);
}

SystemPosition SystemPosition::operator+(const Vec3& other) const {
    return *this + SystemPosition(other);
}

SystemPosition SystemPosition::operator-(const SystemPosition& other) const {
    return SystemPosition(x - other.x, y - other.y, z - other.z);
}

SystemPosition SystemPosition::operator-(const Vec3& other) const {
    return *this - SystemPosition(other);
}

SystemPosition SystemPosition::operator*(float scalar) const {
    return SystemPosition(x * scalar, y * scalar, z * scalar);
}

SystemPosition SystemPosition::operator/(float scalar) const {
    return *this * (1.0f / scalar);
}
}  // namespace dw
