/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "scene/Entity.h"
#include "scene/Component.h"
#include "renderer/BillboardSet.h"
#include "renderer/Mesh.h"
#include "core/math/Defs.h"
#include "core/Delegate.h"

using namespace dw;

class Ship;

class ShipControls : public Component {
public:
    WeakPtr<Ship> ship;
    ClientRpc<Vec3> setLinearVelocity;
    void setLinearVelocityImpl(const Vec3& v) {
        target_linear_velocity = v;
    }
    ClientRpc<Vec3> setAngularVelocity;
    void setAngularVelocityImpl(const Vec3& v) {
        target_angular_velocity = v;
    }

    Vec3 target_linear_velocity;
    Vec3 target_angular_velocity;

    static RepLayout repLayout() {
        return {{},
                {Rpc::bind<ShipControls>(&ShipControls::setLinearVelocity,
                                         &ShipControls::setLinearVelocityImpl),
                 Rpc::bind<ShipControls>(&ShipControls::setAngularVelocity,
                                         &ShipControls::setAngularVelocityImpl)}};
    }
};

class ShipCameraController : public Object {
public:
    DW_OBJECT(ShipCameraController);

    explicit ShipCameraController(Context* ctx, const Vec3& offset);
    ~ShipCameraController() = default;

    void follow(Entity* ship);
    void possess(Entity* camera);

    void update(float dt);

private:
    Entity* possessed_;
    Entity* followed_;

    Vec3 offset_;
};

class ShipEngineData {
public:
    ShipEngineData(const Vec3& force, const Vec3& offset);

    void fire(float power);
    void update(float dt);

    float activity() const;
    Vec3 force() const;
    Vec3 offset() const;

private:
    float activity_;
    Vec3 force_;
    Vec3 offset_;
};

class ShipEngineInstance {
public:
    ShipEngineInstance(ShipEngineData* parent, const Vec3& force, const Vec3& offset,
                       bool forwards);

    Vec3 force() const;
    Vec3 offset() const;
    Vec3 torque() const;
    ShipEngineData* parent() const;
    bool isForwards() const;

private:
    Vec3 force_;
    Vec3 offset_;
    ShipEngineData* parent_;
    bool forwards_;
};

class ShipEngines : public Component, public Object {
public:
    DW_OBJECT(ShipEngines);

    ShipEngines(Context* ctx, const Vector<ShipEngineData>& movement_engines,
                const Vector<ShipEngineData>& nav_engines);

    void onAddToEntity(Entity* parent) override;

    // Movement engines.
    void calculateMaxMovementForce(Vec3& pos_force, Vec3& neg_force);
    // power is proportional to max force.
    Vec3 fireMovementEngines(const Vec3& power);

    // Rotational engines.
    void calculateMaxRotationalTorque(Vec3& clockwise, Vec3& anticlockwise) const;
    // X - pitch, Y - yaw, Z - roll. power is proportional to max torque.
    Vec3 calculateRotationalTorque(const Vec3& power) const;
    Vec3 fireRotationalEngines(const Vec3& power);

    static Vec3 convertToPower(const Vec3& force, const Vec3& max_pos_force,
                               const Vec3& max_neg_force);

    // Replication layout.
    static RepLayout repLayout() {
        return {{RepProperty::bind<ShipEngines>(&ShipEngines::currentMovementPower,
                                                &ShipEngines::rep_setCurrentMovementPower),
                 RepProperty::bind<ShipEngines>(&ShipEngines::currentRotationalPower,
                                                &ShipEngines::rep_setCurrentRotationalPower)},
                {}};
    }

private:
    Vector<ShipEngineData> engine_data_;
    Vector<ShipEngineData> nav_engine_data_;
    SharedPtr<BillboardSet> glow_billboards_;
    SharedPtr<BillboardSet> trail_billboards_;

    // Navigational engines. [0] == x, [1] == y, [2] == z
    Array<Vector<ShipEngineInstance>, 3> movement_engines_;

    // Rotational engines.
    Array<Vector<ShipEngineInstance>, 3> navigation_engines_;

    Vec3 current_movement_power_;
    Vec3 current_rotational_power_;

    // Private replication functions.
    void rep_setCurrentMovementPower(const Vec3& power);
    void rep_setCurrentRotationalPower(const Vec3& power);
    Vec3 currentMovementPower();
    Vec3 currentRotationalPower();

    friend class ShipEngineSystem;
};

class ShipEngineSystem : public System {
public:
    DW_OBJECT(ShipEngineSystem);

    explicit ShipEngineSystem(Context* ctx);

    void processEntity(Entity& entity, float dt) override;
};

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

class Ship : public Object {
public:
    DW_OBJECT(Ship);

    explicit Ship(Context* ctx);
    Ship(Context* ctx, EntityId reserved_entity_id, NetRole role);
    ~Ship() = default;

    void update(float dt);

    // Used by the flight computer.
    void fireMovementThrusters(const Vec3& power);
    void fireRotationalThrusters(const Vec3& power);
    Vec3 angularVelocity() const;
    Vec3 localVelocity() const;

    Entity* entity() const;

private:
    Entity* ship_entity_;
    btRigidBody* rb_;
    SharedPtr<Material> material_;

    SharedPtr<ShipFlightComputer> flight_computer_;
};
