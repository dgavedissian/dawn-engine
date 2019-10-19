/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "Colour.h"

namespace dw {
Colour::Colour() : components_{0.0f, 0.0f, 0.0f, 1.0f} {
}

Colour::Colour(float r, float g, float b, float a) : components_{r, g, b, a} {
}

float Colour::r() const {
    return components_.x;
}

float Colour::g() const {
    return components_.y;
}

float Colour::b() const {
    return components_.z;
}

float Colour::a() const {
    return components_.w;
}

Vec3 Colour::rgb() const {
    return components_.xyz();
}

Vec4 Colour::rgba() const {
    return components_;
}

float& Colour::r() {
    return components_.x;
}

float& Colour::g() {
    return components_.y;
}

float& Colour::b() {
    return components_.z;
}

float& Colour::a() {
    return components_.w;
}

Vec4& Colour::rgba() {
    return components_;
}
}  // namespace dw
