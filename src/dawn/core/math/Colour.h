/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "Defs.h"

namespace dw {
class Colour {
public:
    Colour();
    Colour(float r, float g, float b, float a = 1.0f);

    // Immutable.
    float r() const;
    float g() const;
    float b() const;
    float a() const;
    Vec3 rgb() const;
    Vec4 rgba() const;

    // Mutable.
    float& r();
    float& g();
    float& b();
    float& a();
    Vec4& rgba();

private:
    Vec4 components_;
};
}  // namespace dw
