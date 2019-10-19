/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "Orbit.h"

namespace dw {

CircularOrbit::CircularOrbit(float radius, float period, float phaseDifference /*= 0.0f*/)
    : mRadius(radius), mPeroid(period), mPhaseDiff(phaseDifference) {
}

CircularOrbit::~CircularOrbit() {
}

Position CircularOrbit::calculatePosition(double time) {
    if (mRadius < M_EPSILON) {
        return Position();
    } else {
        double angle = time / (double)mPeroid * (double)math::pi * 2.0 + mPhaseDiff;
        return Position(sin(angle), 0.0, -cos(angle)) * mRadius;
    }
}
}  // namespace dw
