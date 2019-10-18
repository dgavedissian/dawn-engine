/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#pragma once

using namespace dw;

class Ship;

class ShipFlightComputer : public Component {
public:
    explicit ShipFlightComputer(Ship* ship);

    Vec3 target_linear_velocity;
    Vec3 target_angular_velocity;

private:
    Ship* ship_;

    Vec3 ship_acceleration_forwards_;
    Vec3 ship_acceleration_backwards_;
    Vec3 ship_angular_acceleration_forwards_;
    Vec3 ship_angular_acceleration_backwards_;

    friend class ShipFlightComputerSystem;
};

class ShipFlightComputerSystem : public EntitySystem<ShipFlightComputer> {
    void process(float dt) override;
};