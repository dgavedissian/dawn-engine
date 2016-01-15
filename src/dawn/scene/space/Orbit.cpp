/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#include "Common.h"
#include "Orbit.h"

NAMESPACE_BEGIN

CircularOrbit::CircularOrbit(float radius, float period, float phaseDifference /*= 0.0f*/)
    : mRadius(radius), mPeroid(period), mPhaseDiff(phaseDifference)
{
}

CircularOrbit::~CircularOrbit()
{
}

Position CircularOrbit::CalculatePosition(double time)
{
    if (mRadius < M_EPSILON)
    {
        return Position();
    }
    else
    {
        double angle = time / (double)mPeroid * (double)math::pi * 2.0 + mPhaseDiff;
        return Position(sin(angle), 0.0, -cos(angle)) * mRadius;
    }
}

NAMESPACE_END
