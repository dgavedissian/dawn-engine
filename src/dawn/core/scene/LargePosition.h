/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "core/math/Defs.h"

namespace dw {
// A location in the game world, represented as a vector of doubles
// Size: 24 bytes
class DW_API LargePosition {
public:
    LargePosition();
    LargePosition(double x, double y, double z);
    explicit LargePosition(const Vec3& vector);
    LargePosition(const LargePosition& other);

    // Get "local" position relative to a large position.
    Vec3 getRelativeTo(const LargePosition& point) const;

    // Convert to camera space from world space
    Vec3 toCameraSpace(const LargePosition& camera_position) const;

    // Convert to world space from camera space
    static LargePosition fromCameraSpace(const LargePosition& camera_position, const Vec3& point);

    // Operators
    bool operator==(const LargePosition& other) const;
    bool operator!=(const LargePosition& other) const;
    LargePosition& operator=(const LargePosition& other);
    LargePosition& operator=(const Vec3& other);
    LargePosition& operator+=(const LargePosition& other);
    LargePosition& operator+=(const Vec3& other);
    LargePosition& operator-=(const LargePosition& other);
    LargePosition& operator-=(const Vec3& other);
    LargePosition& operator*=(float scalar);
    LargePosition& operator/=(float scalar);
    LargePosition operator-() const;
    LargePosition operator+(const LargePosition& other) const;
    LargePosition operator+(const Vec3& other) const;
    LargePosition operator-(const LargePosition& other) const;
    LargePosition operator-(const Vec3& other) const;
    LargePosition operator*(float scalar) const;
    LargePosition operator/(float scalar) const;

    // Components
    double x;
    double y;
    double z;

    // Origin
    static const LargePosition origin;
};

/// Implementation of Lerp for Position objects
inline LargePosition lerp(const LargePosition& a, const LargePosition& b, float t, float dt) {
    return a + (b - a) * (1.0f - pow(1.0f - t, dt));
}

// Estimate the position to fire at to hit a moving target given a position, speed, target position
// and target velocity
inline LargePosition estimateHit(const LargePosition& position, float speed,
                                 const LargePosition& targetPosition, const Vec3& targetVelocity) {
    Vec3 toTarget = targetPosition.getRelativeTo(position);

    // Here we're solving a quadratic
    float a = targetVelocity.Dot(targetVelocity) - speed * speed;
    float b = 2.0f * targetVelocity.Dot(toTarget);
    float c = toTarget.Dot(toTarget);

    float p = -b / (2.0f * a);
    float q = math::Sqrt(b * b - 4.0f * a * c) / (2.0f * a);
    float t1 = p - q;
    float t2 = p + q;
    float t;

    if (t1 > t2 && t2 > 0) {
        t = t2;
    } else {
        t = t1;
    }

    // Now we have the time before impact - calculate the position at the time
    return targetPosition + targetVelocity * t;
}
}  // namespace dw
