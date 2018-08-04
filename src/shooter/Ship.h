/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "scene/Entity.h"
#include "scene/Component.h"
#include "renderer/BillboardSet.h"
#include "renderer/Mesh.h"
#include "core/math/Defs.h"
#include "core/Delegate.h"
#include "CShipEngines.h"

using namespace dw;

class Ship;

class CShipControls : public Component {
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
                {Rpc::bind<CShipControls>(&CShipControls::setLinearVelocity,
                                          &CShipControls::setLinearVelocityImpl),
                 Rpc::bind<CShipControls>(&CShipControls::setAngularVelocity,
                                          &CShipControls::setAngularVelocityImpl)}};
    }
};

class ShipFlightComputer;

class Ship : public Object {
public:
    DW_OBJECT(Ship);

    Ship(Context* ctx, Frame* frame);
    Ship(Context* ctx, Frame* frame, EntityId reserved_entity_id, NetRole role);
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

    SharedPtr<ShipFlightComputer> flight_computer_;
};
