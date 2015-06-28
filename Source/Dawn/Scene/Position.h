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

NAMESPACE_END
