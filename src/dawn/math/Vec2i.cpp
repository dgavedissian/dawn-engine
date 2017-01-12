/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "math/Vec2i.h"

namespace dw {

Vec2i::Vec2i() : x(0), y(0) {
}

Vec2i::Vec2i(int _x, int _y) : x(_x), y(_y) {
}

Vec2i::Vec2i(const Vec2& v) : x((int)v.x), y((int)v.y) {
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

const Vec2i Vec2i::operator-() const {
    return Vec2i(-x, -y);
}

const Vec2i Vec2i::operator+(const Vec2i& other) const {
    return Vec2i(x + other.x, y + other.y);
}

const Vec2i Vec2i::operator-(const Vec2i& other) const {
    return Vec2i(x - other.x, y - other.y);
}

const Vec2i Vec2i::operator*(int scalar) const {
    return Vec2i(x * scalar, y * scalar);
}

const Vec2i Vec2i::operator/(int scalar) const {
    return Vec2i(x / scalar, y / scalar);
}
}
