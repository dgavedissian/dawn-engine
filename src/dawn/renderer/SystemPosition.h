/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "core/math/Defs.h"

namespace dw {
// A system location in the game world. Should be accurate enough to represent any location in the
class DW_API SystemPosition {
public:
    SystemPosition();
    SystemPosition(double x, double y, double z);
    explicit SystemPosition(const Vec3& vector);
    SystemPosition(const SystemPosition& other);

    // Get "local" position relative to a large position.
    Vec3 getRelativeTo(const SystemPosition& point) const;

    // Convert to camera space from world space
    Vec3 toCameraSpace(const SystemPosition& camera_position) const;

    // Convert to world space from camera space
    static SystemPosition fromCameraSpace(const SystemPosition& camera_position, const Vec3& point);

    // Operators
    bool operator==(const SystemPosition& other) const;
    bool operator!=(const SystemPosition& other) const;
    SystemPosition& operator=(const SystemPosition& other);
    SystemPosition& operator=(const Vec3& other);
    SystemPosition& operator+=(const SystemPosition& other);
    SystemPosition& operator+=(const Vec3& other);
    SystemPosition& operator-=(const SystemPosition& other);
    SystemPosition& operator-=(const Vec3& other);
    SystemPosition& operator*=(float scalar);
    SystemPosition& operator/=(float scalar);
    SystemPosition operator-() const;
    SystemPosition operator+(const SystemPosition& other) const;
    SystemPosition operator+(const Vec3& other) const;
    SystemPosition operator-(const SystemPosition& other) const;
    SystemPosition operator-(const Vec3& other) const;
    SystemPosition operator*(float scalar) const;
    SystemPosition operator/(float scalar) const;

    // Components
    double x;
    double y;
    double z;

    // Origin
    static const SystemPosition origin;
};

/// Implementation of Lerp for Position objects
inline SystemPosition lerp(const SystemPosition& a, const SystemPosition& b, float t, float dt) {
    return a + (b - a) * (1.0f - pow(1.0f - t, dt));
}

// Estimate the position to fire at to hit a moving target given a position, speed, target position
// and target velocity
inline SystemPosition estimateHit(const SystemPosition& position, float speed,
                                  const SystemPosition& targetPosition,
                                  const Vec3& targetVelocity) {
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
