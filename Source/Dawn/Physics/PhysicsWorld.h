/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
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
    void Update(float dt, Camera* camera);

    // Event Delegate
    void HandleEvent(EventDataPtr eventData);

    // Performs a raycast query
    bool RaycastQuery(const Position& start, const Position& end, Camera* camera,
                      PhysicsRaycastResult& result);

private:
    shared_ptr<btBroadphaseInterface> mBroadphase;
    shared_ptr<btCollisionConfiguration> mCollisionConfig;
    shared_ptr<btCollisionDispatcher> mDispatcher;
    shared_ptr<btConstraintSolver> mSolver;
    shared_ptr<btDynamicsWorld> mWorld;
    shared_ptr<BtOgre::DebugDrawer> mDebugDrawer;

    std::list<btRigidBody*> mRigidBodyList;

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
