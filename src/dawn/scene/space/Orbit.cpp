/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "Orbit.h"

namespace dw {

CircularOrbit::CircularOrbit(float radius, float period, float phase_difference /*= 0.0f*/)
    : radius_(radius), period_(period), phase_difference_(phase_difference) {
}

SystemPosition CircularOrbit::calculatePosition(double time) {
    if (radius_ < M_EPSILON) {
        return {};
    } else {
        double angle = time / (double)period_ * (double)math::pi * 2.0 + phase_difference_;
        return SystemPosition{sin(angle), 0.0, -cos(angle)} * radius_;
    }
}
}  // namespace dw
