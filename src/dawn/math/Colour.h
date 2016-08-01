#pragma once

#include "math/Defs.h"

class Colour {
public:
    Colour(float r, float g, float b, float a = 0);
    ~Colour();

private:
    Vec3 mComponents;
};
