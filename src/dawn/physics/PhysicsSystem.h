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
class DW_API PhysicsSystem : public Object {
public:
    DW_OBJECT(PhysicsSystem);

    PhysicsSystem(Context* context);
    ~PhysicsSystem();

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

    void onKey(const KeyEvent& data);

    void addRigidBody(btRigidBody* rigid_body);
    void removeRigidBody(btRigidBody* rigid_body);

    static void onPhysicsTick(btDynamicsWorld* world, btScalar timestep);

    // System for updating RigidBody components.
    class PhysicsComponentSystem : public System {
    public:
        DW_OBJECT(PhysicsComponentSystem);

        PhysicsComponentSystem(Context* context);
        void processEntity(Entity& entity, float dt) override;
    };

    friend class RigidBody;
};

class RigidBody : public Component {
public:
    RigidBody(PhysicsSystem* world, float mass, SharedPtr<btCollisionShape> collision_shape);
    ~RigidBody();

    void onAddToEntity(Entity* parent) override;

    // Set position.

    // Set orientation.

    btRigidBody* _rigidBody() {
        return rigid_body_.get();
    }

private:
    PhysicsSystem* world_;
    UniquePtr<btRigidBody> rigid_body_;
    SharedPtr<btCollisionShape> collision_shape_;
    float mass_;

    friend class PhysicsSystem::PhysicsComponentSystem;
};
}  // namespace dw
// TODO physics events
