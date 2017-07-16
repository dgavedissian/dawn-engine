/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "math/Colour.h"

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
}  // namespace dw
