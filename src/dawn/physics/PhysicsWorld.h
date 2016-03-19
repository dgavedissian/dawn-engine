/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

NAMESPACE_BEGIN

class Renderer;
class RigidEntity;
class Camera;

// Structure to hold the result of a raycast
// TODO: merge this with RendererRaycastResult
struct DW_API PhysicsRaycastResult
{
    RigidEntity* body;
    Position position;
    Vec3 normal;
    bool hit;
};

// Manages the Bullet physics library and provides some helper functions.
class DW_API PhysicsWorld
{
public:
    PhysicsWorld(Renderer* rs);
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
    SharedPtr<BtOgre::DebugDrawer> mDebugDrawer;

    List<btRigidBody*> mRigidBodyList;

    // Adds a btRigidBody to the btDynamicsWorld
    void AddToWorld(btRigidBody* body);

    // Removes a btRigidBody from the btDynamicsWorld
    void RemoveFromWorld(btRigidBody* body);

    // Dispatch collision events
    static void BulletTickCallback(btDynamicsWorld* world, btScalar timestep);

    // Grant RigidEntity access to AddToWorld/RemoveFromWorld
    friend class RigidEntity;
};

NAMESPACE_END
// TODO physics events
