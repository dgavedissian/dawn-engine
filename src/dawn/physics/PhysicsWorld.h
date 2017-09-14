/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once
#include "scene/Position.h"

// Bullet
#include "physics/BulletDynamics.h"

namespace dw {
class Renderer;
class RigidEntity;
class Camera_OLD;

// Structure to hold the result of a raycast
// TODO: merge this with RendererRaycastResult
struct DW_API PhysicsRaycastResult {
    RigidEntity* body;
    Position position;
    Vec3 normal;
    bool hit;
};

// Manages the Bullet physics library and provides some helper functions.
class DW_API PhysicsWorld : public Object {
public:
    DW_OBJECT(PhysicsWorld);

    PhysicsWorld(Context* context);
    ~PhysicsWorld();

    // Takes a step of dt seconds in the simulation
    void update(float dt, Camera_OLD* camera);

    // Performs a raycast query
    bool rayQuery(const Position& start, const Position& end, Camera_OLD* camera,
                  PhysicsRaycastResult& result);

private:
    SharedPtr<btBroadphaseInterface> broadphase_;
    SharedPtr<btCollisionConfiguration> collision_config_;
    SharedPtr<btCollisionDispatcher> dispatcher_;
    SharedPtr<btConstraintSolver> solver_;
    SharedPtr<btDynamicsWorld> world_;

    List<btRigidBody*> rigid_body_list_;

    void onKey(const EvtData_Key& data);

    static void bulletTickCallback(btDynamicsWorld* world, btScalar timestep);

    // Grant RigidEntity access to AddToWorld/RemoveFromWorld
    friend class RigidEntity;
};
}  // namespace dw
// TODO physics events
