/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

namespace dw {

// Interface for different orbit types
class DW_API Orbit {
public:
    virtual ~Orbit(){};

    // Calculate a position given a time
    virtual Position calculatePosition(double time) = 0;
};

// Circular Orbit
// e = 0
class DW_API CircularOrbit : public Orbit {
public:
    CircularOrbit(float radius, float period, float phaseDifference = 0.0f);
    virtual ~CircularOrbit();

    virtual Position calculatePosition(double time) override;

private:
    float mRadius;
    float mPeroid;
    float mPhaseDiff;
};

// Elliptical Orbit
// 0 < e < 1

// Parabolic
// e = 1

// Hyperbolic
// e > 1
}  // namespace dw
