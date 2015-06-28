/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#include "Common.h"
#include "SceneManager.h"
#include "Physics/PhysicsManager.h"
#include "RigidEntity.h"

NAMESPACE_BEGIN

RigidEntity::RigidEntity(SceneManager* sceneMgr) : Entity(sceneMgr)
{
}

RigidEntity::RigidEntity(SceneManager* sceneMgr, shared_ptr<btCollisionShape> shape, float mass,
                         bool disableDeactivation, const Position& position, const Quat& rotation)
    : Entity(sceneMgr)
{
    Init(shape, mass, disableDeactivation, position, rotation);
}

RigidEntity::~RigidEntity()
{
    if (mRigidBody)
        mSceneMgr->GetPhysicsMgr()->RemoveFromWorld(mRigidBody.get());
}

void RigidEntity::Init(shared_ptr<btCollisionShape> shape, float mass, bool disableDeactivation,
                       const Position& position, const Quat& rotation)
{
    // Calculate local inertia
    btVector3 localInertia;
    shape->calculateLocalInertia(mass, localInertia);
    Init(shape, localInertia, mass, disableDeactivation, position, rotation);
}

void RigidEntity::Init(shared_ptr<btCollisionShape> shape, Vec3 localInertia, float mass,
                       bool disableDeactivation, const Position& position, const Quat& rotation)
{
    // Create the starting motion state
    btTransform startTransform(rotation, btVector3());
    mMotionState = make_shared<RigidEntityMotionState>(startTransform, this);
    mCollisionShape = shape;

    // Create the rigid body
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, mMotionState.get(), shape.get(),
                                                    localInertia);
    mRigidBody.reset(new btRigidBody(rbInfo));
    if (disableDeactivation)
        mRigidBody->setActivationState(DISABLE_DEACTIVATION);

    mRigidBody->setUserPointer((void*)this);
    mRigidBody->setDamping(0.0f, 0.0f);
    mSceneMgr->GetPhysicsMgr()->AddToWorld(mRigidBody.get());
}

void RigidEntity::SetPosition(const Position& position)
{
    Entity::SetPosition(position);
}

void RigidEntity::SetOrientation(const Quat& orient)
{
    Entity::SetOrientation(orient);
    mRigidBody->getWorldTransform().setRotation(orient);
}

void RigidEntity::PreSimulationStep(Camera* camera)
{
    // Set the position of the rigid body to the camera-space position
    mLastPosition = mPosition.ToCameraSpace(camera);
    mRigidBody->getWorldTransform().setOrigin(mLastPosition);
}

void RigidEntity::UpdateTransform(const btTransform& xform)
{
    SetPosition(mPosition + xform.getOrigin() - mLastPosition);
    mOrientation = xform.getRotation();
}

NAMESPACE_END
