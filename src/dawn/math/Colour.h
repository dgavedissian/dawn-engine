/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "math/Defs.h"

class Colour {
public:
    Colour(float r, float g, float b, float a = 1.0f);

private:
    Vec4 mComponents;
};
