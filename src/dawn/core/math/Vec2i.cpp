/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "Vec2i.h"

namespace dw {

Vec2i::Vec2i() : x(0), y(0) {
}

Vec2i::Vec2i(int _x, int _y) : x(_x), y(_y) {
}

Vec2i::Vec2i(const Vec2& v) : x(static_cast<int>(v.x)), y(static_cast<int>(v.y)) {
}

Vec2i& Vec2i::operator=(const Vec2i& other) {
    x = other.x;
    y = other.y;
    return *this;
}

Vec2i& Vec2i::operator+=(const Vec2i& other) {
    x += other.x;
    y += other.y;
    return *this;
}

Vec2i& Vec2i::operator-=(const Vec2i& other) {
    x -= other.x;
    y -= other.y;
    return *this;
}

Vec2i& Vec2i::operator*=(int scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
}

Vec2i& Vec2i::operator/=(int scalar) {
    x /= scalar;
    y /= scalar;
    return *this;
}

Vec2i& Vec2i::operator*=(float scalar) {
    x = static_cast<int>(x * scalar);
    y = static_cast<int>(y * scalar);
    return *this;
}

Vec2i& Vec2i::operator/=(float scalar) {
    x = static_cast<int>(x * scalar);
    y = static_cast<int>(y / scalar);
    return *this;
}

Vec2i& Vec2i::operator*=(const Vec2& other) {
    x = static_cast<int>(x * other.x);
    y = static_cast<int>(y * other.y);
    return *this;
}

Vec2i& Vec2i::operator/=(const Vec2& other) {
    x = static_cast<int>(x / other.x);
    y = static_cast<int>(y / other.y);
    return *this;
}

Vec2i Vec2i::operator-() const {
    return Vec2i(-x, -y);
}

Vec2i Vec2i::operator+(const Vec2i& other) const {
    return Vec2i(x + other.x, y + other.y);
}

Vec2i Vec2i::operator-(const Vec2i& other) const {
    return Vec2i(x - other.x, y - other.y);
}

Vec2i Vec2i::operator*(int scalar) const {
    return Vec2i(x * scalar, y * scalar);
}

Vec2i Vec2i::operator/(int scalar) const {
    return Vec2i(x / scalar, y / scalar);
}

Vec2i Vec2i::operator*(float scalar) const {
    return Vec2i(static_cast<int>(x * scalar), static_cast<int>(y * scalar));
}

Vec2i Vec2i::operator/(float scalar) const {
    return Vec2i(static_cast<int>(x / scalar), static_cast<int>(y / scalar));
}

Vec2i Vec2i::operator*(const Vec2& other) const {
    return Vec2i(static_cast<int>(x * other.x), static_cast<int>(y * other.y));
}

Vec2i Vec2i::operator/(const Vec2& other) const {
    return Vec2i(static_cast<int>(x / other.x), static_cast<int>(y / other.y));
}
}  // namespace dw
