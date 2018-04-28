/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

using namespace dw;

class Ship;

class ShipFlightComputer : public Object {
public:
    DW_OBJECT(ShipFlightComputer);

    ShipFlightComputer(Context* ctx, Ship* ship);

    void setTargetLinearVelocity(const Vec3& target_linear_velocity);
    void setTargetAngularVelocity(const Vec3& target_angular_velocity);

    void update(float dt);

private:
    Ship* ship_;
    Vec3 target_linear_velocity_;
    Vec3 target_angular_velocity_;

    Vec3 ship_acceleration_forwards_;
    Vec3 ship_acceleration_backwards_;
    Vec3 ship_angular_acceleration_forwards_;
    Vec3 ship_angular_acceleration_backwards_;
};