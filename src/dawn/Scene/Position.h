/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

NAMESPACE_BEGIN

class Camera;

// A location in the game world, represented as a vector of doubles
// Size: 24 bytes
class DW_API Position
{
public:
    Position();
    Position(double _x, double _y, double _z);
    Position(const Vec3& vector);
    Position(const Position& other);

    // Get relative position from a point
    Vec3 GetRelativeToPoint(const Position& point) const;

    // Convert to camera space from world space
    Vec3 ToCameraSpace(Camera* camera) const;

    // Convert to world space from camera space
    static Position FromCameraSpace(Camera* camera, const Vec3& cameraSpace);

    // Operators
    bool operator==(const Position& other) const;
    bool operator!=(const Position& other) const;
    Position& operator=(const Position& other);
    Position& operator=(const Vec3& other);
    Position& operator+=(const Position& other);
    Position& operator+=(const Vec3& other);
    Position& operator-=(const Position& other);
    Position& operator-=(const Vec3& other);
    Position& operator*=(float scalar);
    Position& operator/=(float scalar);
    const Position operator-() const;
    const Position operator+(const Position& other) const;
    const Position operator+(const Vec3& other) const;
    const Position operator-(const Position& other) const;
    const Position operator-(const Vec3& other) const;
    const Position operator*(float scalar) const;
    const Position operator/(float scalar) const;

    // Components
    double x;
    double y;
    double z;

    // Origin
    static const Position origin;
};

/// Implementation of Lerp for Position objects
inline Position Lerp(const Position& a, const Position& b, float t, float dt)
{
	return a + (b - a) * (1.0f - Pow(1.0f - t, dt));
}

// Estimate the position to fire at to hit a moving target given a position, speed, target position
// and target velocity
inline Position EstimateHitPosition(const Position& position, float speed,
									const Position& targetPosition, const Vec3& targetVelocity)
{
	Vec3 toTarget = targetPosition.GetRelativeToPoint(position);

	// Here we're solving a quadratic
	float a = targetVelocity.Dot(targetVelocity) - speed * speed;
	float b = 2.0f * targetVelocity.Dot(toTarget);
	float c = toTarget.Dot(toTarget);

	float p = -b / (2.0f * a);
	float q = math::Sqrt((b * b) - 4.0f * a * c) / (2.0f * a);
	float t1 = p - q;
	float t2 = p + q;
	float t;

	if (t1 > t2 && t2 > 0)
		t = t2;
	else
		t = t1;

	// Now we have the time before impact - calculate the position at the time
	return targetPosition + targetVelocity * t;
}


NAMESPACE_END
