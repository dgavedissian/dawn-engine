/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#pragma once

#include "renderer/SystemPosition.h"
#include "renderer/Node.h"
#include "scene/SceneManager.h"
#include "input/Input.h"

// Bullet
#include "scene/BulletDynamics.h"

namespace dw {
class Renderer;
class RigidEntity;

// Structure to hold the result of a raycast
// TODO: merge this with RendererRaycastResult
struct DW_API PhysicsRaycastResult {
    RigidEntity* body;
    SystemPosition position;
    Vec3 normal;
    bool hit;
};

class CRigidBody;

// Manages the Bullet physics library and provides some helper functions.
class DW_API PhysicsScene : public Object {
public:
    DW_OBJECT(PhysicsScene);

    PhysicsScene(Context* context, SceneManager* scene_mgr, EventSystem* event_system);
    ~PhysicsScene();

    // Takes a step of dt seconds in the simulation
    void update(float dt, SystemNode* camera);

    // Performs a raycast query
    bool rayQuery(const SystemPosition& start, const SystemPosition& end, SystemNode* camera,
                  PhysicsRaycastResult& result);

    // EntitySystem for updating CRigidBody components.
    class PhysicsComponentSystem : public EntitySystem<CSceneNode, CRigidBody> {
    public:
        void process(float dt) override;
    };

private:
    EventSystem* event_system_;
    UniquePtr<btBroadphaseInterface> broadphase_;
    UniquePtr<btCollisionConfiguration> collision_config_;
    UniquePtr<btCollisionDispatcher> dispatcher_;
    UniquePtr<btConstraintSolver> solver_;
    UniquePtr<btDynamicsWorld> world_;

    List<btRigidBody*> rigid_body_list_;

    void onKey(const KeyEvent& data);

    void addRigidBody(btRigidBody* rigid_body);
    void removeRigidBody(btRigidBody* rigid_body);

    static void onPhysicsTick(btDynamicsWorld* world, btScalar timestep);

    friend class CRigidBody;
};

class CRigidBody : public Component {
public:
    CRigidBody(PhysicsScene* world, float mass, SharedPtr<btCollisionShape> collision_shape);
    ~CRigidBody();

    void onAddToEntity(Entity* parent) override;

    // Set position.

    // Set orientation.

    btRigidBody* _rigidBody() {
        return rigid_body_.get();
    }

private:
    PhysicsScene* world_;
    SharedPtr<btRigidBody> rigid_body_;
    SharedPtr<btCollisionShape> collision_shape_;
    float mass_;

    friend class PhysicsScene::PhysicsComponentSystem;
};
}  // namespace dw
// TODO physics events
