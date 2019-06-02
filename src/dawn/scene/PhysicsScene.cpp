/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "input/Input.h"
#include "scene/SceneManager.h"
#include "PhysicsScene.h"

namespace dw {
namespace {
btTransform toBulletTransform(detail::Transform& xform) {
    btQuaternion quat{xform.orientation.x, xform.orientation.y, xform.orientation.z,
                      xform.orientation.w};
    return btTransform(
        quat, {static_cast<btScalar>(xform.position.x), static_cast<btScalar>(xform.position.y),
               static_cast<btScalar>(xform.position.z)});
}

void fromBulletTransform(const btTransform& source, detail::Transform& dest) {
    btQuaternion rotation;
    source.getBasis().getRotation(rotation);
    dest.position = Vec3{source.getOrigin().x(), source.getOrigin().y(), source.getOrigin().z()};
    dest.orientation = Quat{rotation.x(), rotation.y(), rotation.z(), rotation.w()};
}
}  // namespace
PhysicsScene::PhysicsScene(Context* context, SceneManager* scene_mgr, EventSystem* event_system)
    : Object(context), event_system_(event_system) {
    log().info("Bullet Version %s.%s", btGetVersion() / 100, btGetVersion() % 100);

    broadphase_ = makeUnique<btDbvtBroadphase>();
    collision_config_ = makeUnique<btDefaultCollisionConfiguration>();
    dispatcher_ = makeUnique<btCollisionDispatcher>(collision_config_.get());
    solver_ = makeUnique<btSequentialImpulseConstraintSolver>();
    world_ = makeUnique<btDiscreteDynamicsWorld>(dispatcher_.get(), broadphase_.get(),
                                                 solver_.get(), collision_config_.get());

    // Set the properties of the world.
    world_->setGravity(btVector3(0.0f, 0.0f, 0.0f));
    world_->setInternalTickCallback(onPhysicsTick);

    // Register delegates.
    event_system->addListener(this, &PhysicsScene::onKey);

    scene_mgr->addSystem<PhysicsComponentSystem>();
}

PhysicsScene::~PhysicsScene() {
    event_system_->removeAllListeners(this);
    world_.reset();
    solver_.reset();
    dispatcher_.reset();
    collision_config_.reset();
    broadphase_.reset();
}

void PhysicsScene::update(float dt, SystemNode*) {
    world_->stepSimulation(dt, 5);
    // mDebugDrawer->step();
}

bool PhysicsScene::rayQuery(const SystemPosition& start, const SystemPosition& end,
                            SystemNode* camera, PhysicsRaycastResult& result) {
    // Make sure this is done in camera-space
    btVector3 start_cs = start.toCameraSpace(camera->position);
    btVector3 end_cs = end.toCameraSpace(camera->position);

    // Ensure that the direction can be normalised
    btVector3 delta = end_cs - start_cs;
    if (!delta.fuzzyZero()) {
        btCollisionWorld::ClosestRayResultCallback raycast(start_cs, end_cs);
        world_->rayTest(start_cs, end_cs, raycast);

        // Fill the result structure
        result.position =
            SystemPosition::fromCameraSpace(camera->position, raycast.m_hitPointWorld);
        result.normal = raycast.m_hitNormalWorld;
        result.hit = raycast.hasHit();

        if (raycast.m_collisionObject) {
            result.body = static_cast<RigidEntity*>(
                btRigidBody::upcast(raycast.m_collisionObject)->getUserPointer());
        } else {
            result.body = nullptr;
        }
    } else {
        result.position = Vec3::zero;
        result.normal = Vec3::zero;
        result.hit = false;
        result.body = nullptr;
    }

    // Has the raycast hit something?
    return result.hit;
}

void PhysicsScene::onKey(const KeyEvent& data) {
    if (data.key == Key::F2 && data.down) {
        // mDebugDrawer->setDebugMode(!mDebugDrawer->getDebugMode());
    }
}

void PhysicsScene::onPhysicsTick(btDynamicsWorld* /*world*/, btScalar /*timestep*/) {
}

void PhysicsScene::addRigidBody(btRigidBody* rigid_body) {
    if (world_) {
        world_->addRigidBody(rigid_body);
    }
}

void PhysicsScene::removeRigidBody(btRigidBody* rigid_body) {
    if (world_) {
        world_->removeRigidBody(rigid_body);
    }
}

void PhysicsScene::PhysicsComponentSystem::process(float) {
    entityView().each([](auto entity, const auto& node, auto& rigid_body) {
        fromBulletTransform(rigid_body.rigid_body_->getWorldTransform(), node.node->transform());
    });
}

CRigidBody::CRigidBody(PhysicsScene* world, float mass, SharedPtr<btCollisionShape> collision_shape)
    : world_{world},
      rigid_body_{nullptr},
      collision_shape_{std::move(collision_shape)},
      mass_{mass} {
}

CRigidBody::~CRigidBody() {
    if (rigid_body_) {
        world_->removeRigidBody(rigid_body_.get());
        delete rigid_body_->getMotionState();
        rigid_body_.reset();
    }
}

void CRigidBody::onAddToEntity(Entity* parent) {
    // Get initial transform.
    assert(parent->transform());
    btTransform initial_transform = toBulletTransform(*parent->transform());

    // Set up rigid body.
    btVector3 inertia;
    collision_shape_->calculateLocalInertia(mass_, inertia);
    btRigidBody::btRigidBodyConstructionInfo rigid_body_ci(
        mass_, new btDefaultMotionState(initial_transform), collision_shape_.get(), inertia);
    rigid_body_ = makeUnique<btRigidBody>(rigid_body_ci);
    rigid_body_->setDamping(0.0f, 0.0f);
    world_->addRigidBody(rigid_body_.get());
}
}  // namespace dw
