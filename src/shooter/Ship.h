/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
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

    Vec3 target_linear_velocity;
    Vec3 target_angular_velocity;
    bool firing_weapon;

    void setLinearVelocityImpl(const Vec3& v) {
        target_linear_velocity = v;
    }
    void setAngularVelocityImpl(const Vec3& v) {
        target_angular_velocity = v;
    }
    void toggleWeaponImpl(const bool& toggle) {
        firing_weapon = toggle;
    }

    ClientRpc<Vec3> setLinearVelocity = &CShipControls::setLinearVelocityImpl;
    ClientRpc<Vec3> setAngularVelocity = &CShipControls::setAngularVelocityImpl;
    ClientRpc<bool> toggleWeapon = &CShipControls::toggleWeaponImpl;

    static RepLayout repLayout() {
        return {{},
                {BindRpc<CShipControls, RpcType::Client, Vec3>(&CShipControls::setLinearVelocity),
                 BindRpc<CShipControls, RpcType::Client, Vec3>(&CShipControls::setAngularVelocity),
                 BindRpc<CShipControls, RpcType::Client, bool>(&CShipControls::toggleWeapon)}};
    }
};

class ShipFlightComputer;

class Ship : public Object {
public:
    DW_OBJECT(Ship);

    Ship(Context* ctx, NetInstance* net, SceneManager* scene_manager, Frame* frame);
    Ship(Context* ctx, NetInstance* net, SceneManager* scene_manager, Frame* frame,
         EntityId reserved_entity_id, NetRole role);
    ~Ship() = default;

    void update(float dt);

    void fireWeapon();

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
