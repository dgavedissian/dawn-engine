/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "input/Input.h"
#include "PhysicsWorld.h"

namespace dw {

PhysicsWorld::PhysicsWorld(Context* context) : Object(context) {
    log().info("Bullet Version %s.%s", btGetVersion() / 100, btGetVersion() % 100);

    broadphase_.reset(new btDbvtBroadphase());
    collision_config_.reset(new btDefaultCollisionConfiguration());
    dispatcher_.reset(new btCollisionDispatcher(collision_config_.get()));
    solver_.reset(new btSequentialImpulseConstraintSolver);
    world_.reset(new btDiscreteDynamicsWorld(dispatcher_.get(), broadphase_.get(), solver_.get(),
                                             collision_config_.get()));

    // Set the properties of the world
    world_->setGravity(btVector3(0.0f, 0.0f, 0.0f));
    world_->setInternalTickCallback(bulletTickCallback);

    // Register delegates.
    addEventListener<EvtData_KeyDown>(makeEventDelegate(this, &PhysicsWorld::onKeyDown));
}

PhysicsWorld::~PhysicsWorld() {
    removeEventListener<EvtData_KeyDown>(makeEventDelegate(this, &PhysicsWorld::onKeyDown));

    log().info("Bullet cleaned up");
}

void PhysicsWorld::update(float dt, Camera_OLD*) {
    // Call PreSimulationStep on each rigid body
    // for (auto body : rigid_body_list_)
    //   static_cast<RigidEntity*>(body->getUserPointer())->PreSimulationStep(camera);

    // Step the simulation
    world_->stepSimulation(dt, 0);
    // mDebugDrawer->step();
}

void PhysicsWorld::onKeyDown(const EvtData_KeyDown& data) {
    if (data.key == Key::F2) {
        // mDebugDrawer->setDebugMode(!mDebugDrawer->getDebugMode());
    }
}

bool PhysicsWorld::rayQuery(const Position& start, const Position& end, Camera_OLD* camera,
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

void PhysicsWorld::bulletTickCallback(btDynamicsWorld* /*world*/, btScalar /*timestep*/) {
}
}  // namespace dw
