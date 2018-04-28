/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "Defs.h"

namespace dw {
class Colour {
public:
    Colour();
    Colour(float r, float g, float b, float a = 1.0f);

    float r() const;
    float g() const;
    float b() const;
    float a() const;
    Vec3 rgb() const;
    Vec4 rgba() const;

private:
    Vec4 components_;
};
}  // namespace dw
