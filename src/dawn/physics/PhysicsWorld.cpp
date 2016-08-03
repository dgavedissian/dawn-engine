/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "input/Input.h"
#include "PhysicsWorld.h"

namespace dw {

PhysicsWorld::PhysicsWorld() {
    LOG << "Bullet Version " << (btGetVersion() / 100) << "." << (btGetVersion() % 100);

    mBroadphase.reset(new btDbvtBroadphase());
    mCollisionConfig.reset(new btDefaultCollisionConfiguration());
    mDispatcher.reset(new btCollisionDispatcher(mCollisionConfig.get()));
    mSolver.reset(new btSequentialImpulseConstraintSolver);
    mWorld.reset(new btDiscreteDynamicsWorld(mDispatcher.get(), mBroadphase.get(), mSolver.get(),
                                             mCollisionConfig.get()));

    // Set the properties of the world
    mWorld->setGravity(btVector3(0.0f, 0.0f, 0.0f));
    mWorld->setInternalTickCallback(bulletTickCallback);

    // Register event delegates
    ADD_LISTENER(PhysicsWorld, EvtData_KeyDown);
}

PhysicsWorld::~PhysicsWorld() {
    REMOVE_LISTENER(PhysicsWorld, EvtData_KeyDown);

    LOG << "Bullet cleaned up";
}

void PhysicsWorld::update(float dt, Camera* camera) {
    // Call PreSimulationStep on each rigid body
    //for (auto body : mRigidBodyList)
    //   static_cast<RigidEntity*>(body->getUserPointer())->PreSimulationStep(camera);

    // Step the simulation
    mWorld->stepSimulation(dt, 0);
    //mDebugDrawer->step();
}

void PhysicsWorld::handleEvent(EventDataPtr eventData) {
    if (eventIs<EvtData_KeyDown>(eventData)) {
        auto castedEventData = castEvent<EvtData_KeyDown>(eventData);
        if (castedEventData->key == Key::F2)
            ;//mDebugDrawer->setDebugMode(!mDebugDrawer->getDebugMode());
    }
}

bool PhysicsWorld::rayQuery(const Position& start, const Position& end, Camera* camera,
                            PhysicsRaycastResult& result) {
    // Make sure this is done in camera-space
    btVector3 startCS = start.toCameraSpace(camera);
    btVector3 endCS = end.toCameraSpace(camera);

    // Ensure that the direction can be normalised
    btVector3 delta = endCS - startCS;
    if (!delta.fuzzyZero()) {
        btCollisionWorld::ClosestRayResultCallback raycast(startCS, endCS);
        mWorld->rayTest(startCS, endCS, raycast);

        // Fill the result structure
        result.position = Position::fromCameraSpace(camera, raycast.m_hitPointWorld);
        result.normal = raycast.m_hitNormalWorld;
        result.hit = raycast.hasHit();

        if (raycast.m_collisionObject)
            result.body = static_cast<RigidEntity*>(
                btRigidBody::upcast(raycast.m_collisionObject)->getUserPointer());
        else
            result.body = nullptr;
    } else {
        result.position = Vec3::zero;
        result.normal = Vec3::zero;
        result.hit = false;
        result.body = nullptr;
    }

    // Has the raycast hit something?
    return result.hit;
}

void PhysicsWorld::bulletTickCallback(btDynamicsWorld* world, btScalar timestep) {
}
}
