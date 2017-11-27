/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "input/Input.h"
#include "scene/System.h"
#include "scene/SystemManager.h"
#include "PhysicsSystem.h"

namespace dw {
namespace {
btTransform toBulletTransform(Transform& xform) {
    btQuaternion quat{xform.orientation().x, xform.orientation().y, xform.orientation().z,
                      xform.orientation().w};
    return btTransform(
        quat, {static_cast<btScalar>(xform.position().x), static_cast<btScalar>(xform.position().y),
               static_cast<btScalar>(xform.position().z)});
}

void fromBulletTransform(const btTransform& source, Transform& dest) {
    btQuaternion rotation;
    source.getBasis().getRotation(rotation);
    dest.position() =
        Position{source.getOrigin().x(), source.getOrigin().y(), source.getOrigin().z()};
    dest.orientation() = Quat{rotation.x(), rotation.y(), rotation.z(), rotation.w()};
}
}  // namespace
PhysicsSystem::PhysicsSystem(Context* context) : Subsystem(context) {
    setDependencies<SystemManager>();

    log().info("Bullet Version %s.%s", btGetVersion() / 100, btGetVersion() % 100);

    broadphase_.reset(new btDbvtBroadphase());
    collision_config_.reset(new btDefaultCollisionConfiguration());
    dispatcher_.reset(new btCollisionDispatcher(collision_config_.get()));
    solver_.reset(new btSequentialImpulseConstraintSolver);
    world_.reset(new btDiscreteDynamicsWorld(dispatcher_.get(), broadphase_.get(), solver_.get(),
                                             collision_config_.get()));

    // Set the properties of the world.
    world_->setGravity(btVector3(0.0f, 0.0f, 0.0f));
    world_->setInternalTickCallback(onPhysicsTick);

    // Register delegates.
    addEventListener<KeyEvent>(makeEventDelegate(this, &PhysicsSystem::onKey));

    subsystem<SystemManager>()->addSystem<PhysicsComponentSystem>();
}

PhysicsSystem::~PhysicsSystem() {
    removeEventListener<KeyEvent>(makeEventDelegate(this, &PhysicsSystem::onKey));
}

void PhysicsSystem::update(float dt, Camera_OLD*) {
    world_->stepSimulation(dt, 5);
    // mDebugDrawer->step();
}

bool PhysicsSystem::rayQuery(const Position& start, const Position& end, Camera_OLD* camera,
                             PhysicsRaycastResult& result) {
    // Make sure this is done in camera-space
    btVector3 start_cs = start.toCameraSpace(camera);
    btVector3 end_cs = end.toCameraSpace(camera);

    // Ensure that the direction can be normalised
    btVector3 delta = end_cs - start_cs;
    if (!delta.fuzzyZero()) {
        btCollisionWorld::ClosestRayResultCallback raycast(start_cs, end_cs);
        world_->rayTest(start_cs, end_cs, raycast);

        // Fill the result structure
        result.position = Position::fromCameraSpace(camera, raycast.m_hitPointWorld);
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

void PhysicsSystem::onKey(const KeyEvent& data) {
    if (data.key == Key::F2 && data.down) {
        // mDebugDrawer->setDebugMode(!mDebugDrawer->getDebugMode());
    }
}

void PhysicsSystem::onPhysicsTick(btDynamicsWorld* /*world*/, btScalar /*timestep*/) {
}

void PhysicsSystem::addRigidBody(btRigidBody* rigid_body) {
    world_->addRigidBody(rigid_body);
}

void PhysicsSystem::removeRigidBody(btRigidBody* rigid_body) {
    world_->removeRigidBody(rigid_body);
}

PhysicsSystem::PhysicsComponentSystem::PhysicsComponentSystem(Context* context) : System(context) {
    supportsComponents<Transform, RigidBody>();
}

void PhysicsSystem::PhysicsComponentSystem::processEntity(Entity& entity, float) {
    auto t = entity.component<Transform>();
    auto rb = entity.component<RigidBody>()->rigid_body_.get();
    fromBulletTransform(rb->getWorldTransform(), *t);
}

RigidBody::RigidBody(PhysicsSystem* world, float mass, SharedPtr<btCollisionShape> collision_shape)
    : world_{world}, collision_shape_{std::move(collision_shape)}, mass_{mass} {
}

RigidBody::~RigidBody() {
    if (rigid_body_) {
        world_->removeRigidBody(rigid_body_.get());
        delete rigid_body_->getMotionState();
        rigid_body_.reset();
    }
}

void RigidBody::onAddToEntity(Entity* parent) {
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
