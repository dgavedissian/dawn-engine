/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "renderer/SystemPosition.h"

namespace dw {

// TODO: Refactor this to instead be a "OrbitDesc" struct with different static factory methods like
// "createCircularOrbit" / "createEllipticalOrbit"

// Interface for different orbit types
class DW_API Orbit {
public:
    virtual ~Orbit() = default;

    // Calculate a position given a time
    virtual SystemPosition calculatePosition(double time) = 0;
};

// Circular Orbit
// e = 0
class DW_API CircularOrbit : public Orbit {
public:
    CircularOrbit(float radius, float period, float phase_difference = 0.0f);
    ~CircularOrbit() override = default;

    SystemPosition calculatePosition(double time) override;

private:
    float radius_;
    float period_;
    float phase_difference_;
};

// Elliptical Orbit
// 0 < e < 1

// Parabolic
// e = 1

// Hyperbolic
// e > 1
}  // namespace dw
