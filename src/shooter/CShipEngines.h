/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "scene/Entity.h"
#include "scene/Component.h"
#include "renderer/BillboardSet.h"
#include "renderer/Mesh.h"
#include "core/math/Defs.h"
#include "core/Delegate.h"

using namespace dw;

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

class CShipEngines : public Component, public Object {
public:
    DW_OBJECT(CShipEngines);

    CShipEngines(Context* ctx, const Vector<ShipEngineData>& movement_engines,
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
        return {{RepProperty::bind<CShipEngines>(&CShipEngines::currentMovementPower,
                                                 &CShipEngines::rep_setCurrentMovementPower),
                 RepProperty::bind<CShipEngines>(&CShipEngines::currentRotationalPower,
                                                 &CShipEngines::rep_setCurrentRotationalPower)},
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

    friend class SShipEngines;
};

class SShipEngines : public EntitySystem<CSceneNode, CShipEngines> {
public:
    void process(float dt) override;
};
