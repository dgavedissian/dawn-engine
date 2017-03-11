/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once
#include "scene/Position.h"

namespace dw {
class Renderer;
class RigidEntity;
class Camera;

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
    void update(float dt, Camera* camera);

    // Event Delegate
    void handleEvent(EventDataPtr eventData);

    // Performs a raycast query
    bool rayQuery(const Position& start, const Position& end, Camera* camera,
                  PhysicsRaycastResult& result);

private:
    SharedPtr<btBroadphaseInterface> mBroadphase;
    SharedPtr<btCollisionConfiguration> mCollisionConfig;
    SharedPtr<btCollisionDispatcher> mDispatcher;
    SharedPtr<btConstraintSolver> mSolver;
    SharedPtr<btDynamicsWorld> mWorld;

    List<btRigidBody*> mRigidBodyList;

    // Dispatch collision events
    static void bulletTickCallback(btDynamicsWorld* world, btScalar timestep);

    // Grant RigidEntity access to AddToWorld/RemoveFromWorld
    friend class RigidEntity;
};
}
// TODO physics events
