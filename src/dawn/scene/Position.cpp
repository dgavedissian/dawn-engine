/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
//#include "Camera.h"
#include "Position.h"

namespace dw {

const Position Position::origin;

Position::Position() : x{0.0}, y{0.0}, z{0.0} {
}

Position::Position(double x, double y, double z) : x{x}, y{y}, z{z} {
}

Position::Position(const Vec3& vector)
    : x(static_cast<double>(vector.x)),
      y(static_cast<double>(vector.y)),
      z(static_cast<double>(vector.z)) {
}

Position::Position(const Position& other) : x{other.x}, y{other.y}, z{other.z} {
}

Vec3 Position::getRelativeTo(const Position& point) const {
    Position delta = *this - point;
    return Vec3(static_cast<float>(delta.x), static_cast<float>(delta.y),
                static_cast<float>(delta.z));
}

Vec3 Position::toCameraSpace(Camera* camera) const {
    // TODO(David): stub
    return getRelativeTo(*this);
    // return getRelativeTo(camera->getPosition());
}

Position Position::fromCameraSpace(Camera* camera, const Vec3& cameraSpace) {
    // TODO(David): stub
    return Position(cameraSpace);
    // return camera->getPosition() + cameraSpace;
}

bool Position::operator==(const Position& other) const {
    return floatEq(x, other.x) && floatEq(y, other.y) && floatEq(z, other.z);
}

bool Position::operator!=(const Position& other) const {
    return !(*this == other);
}

Position& Position::operator=(const Position& other) {
    x = other.x;
    y = other.y;
    z = other.z;
    return *this;
}

Position& Position::operator=(const Vec3& other) {
    return *this = Position(other);
}

Position& Position::operator+=(const Position& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Position& Position::operator+=(const Vec3& other) {
    return *this += Position(other);
}

Position& Position::operator-=(const Position& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Position& Position::operator-=(const Vec3& other) {
    return *this -= Position(other);
}

Position& Position::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Position& Position::operator/=(float scalar) {
    return *this *= (1.0f / scalar);
}

Position Position::operator-() const {
    return Position(-x, -y, -z);
}

Position Position::operator+(const Position& other) const {
    return Position(x + other.x, y + other.y, z + other.z);
}

Position Position::operator+(const Vec3& other) const {
    return *this + Position(other);
}

Position Position::operator-(const Position& other) const {
    return Position(x - other.x, y - other.y, z - other.z);
}

Position Position::operator-(const Vec3& other) const {
    return *this - Position(other);
}

Position Position::operator*(float scalar) const {
    return Position(x * scalar, y * scalar, z * scalar);
}

Position Position::operator/(float scalar) const {
    return *this * (1.0f / scalar);
}
}  // namespace dw
