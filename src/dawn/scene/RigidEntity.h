/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "Entity.h"

NAMESPACE_BEGIN

// Specialised entity that is affected by physical dynamics
class DW_API RigidEntity : public Entity
{
public:
    RigidEntity(SceneManager* sceneMgr);
    RigidEntity(SceneManager* sceneMgr,SharedPtr<btCollisionShape> shape, float mass,
                bool disableDeactivation, const Position& position = Position::origin,
                const Quat& rotation = Quat::identity);
    virtual ~RigidEntity();

    // Construct the object - I'm not really a fan of two-stage init but this is enforced if we want
    // to cache entities or use RakNet::Replica3
    void Init(SharedPtr<btCollisionShape> shape, float mass, bool disableDeactivation,
              const Position& position = Position::origin, const Quat& rotation = Quat::identity);
    void Init(SharedPtr<btCollisionShape> shape, Vec3 localInertia, float mass,
              bool disableDeactivation, const Position& position = Position::origin,
              const Quat& rotation = Quat::identity);

    // Accessors
    SharedPtr<btCollisionShape> GetBulletCollisionShape() { return mCollisionShape; }
    SharedPtr<btRigidBody> GetBulletRigidBody() { return mRigidBody; }
    SharedPtr<btMotionState> GetBulletMotionState() { return mMotionState; }
    Vec3 GetLinearVelocity() const
    {
        return mRigidBody ? Vec3(mRigidBody->getLinearVelocity()) : Vec3::zero;
    }
    Vec3 GetAngularVelocity() const
    {
        return mRigidBody ? Vec3(mRigidBody->getAngularVelocity()) : Vec3::zero;
    }

    // Inherited from Entity
    virtual void SetPosition(const Position& position) override;
    virtual void SetOrientation(const Quat& orient) override;

protected:
    SharedPtr<btCollisionShape> mCollisionShape;
    SharedPtr<btRigidBody> mRigidBody;
    SharedPtr<btMotionState> mMotionState;
    btVector3 mLastPosition;

    // Called by PhysicsManager before the simulation step
    void PreSimulationStep(Camera* camera);

    // Called by PhysicsManager after simulation step
    void UpdateTransform(const btTransform& xform);

private:
    // Motion State
    class RigidEntityMotionState : public btMotionState
    {
    public:
        RigidEntityMotionState(const btTransform& initialTransform, RigidEntity* entity)
        {
            mEntity = entity;
            mTransform = initialTransform;
        }

        virtual ~RigidEntityMotionState() {}

        // Inherited from btMotionState
        virtual void getWorldTransform(btTransform& worldTrans) const override
        {
            worldTrans = mTransform;
        }

        virtual void setWorldTransform(const btTransform& worldTrans) override
        {
            mEntity->UpdateTransform(worldTrans);
        }

    protected:
        RigidEntity* mEntity;
        btTransform mTransform;
    };

    // Give PhysicsManager access to the private methods
    friend class PhysicsWorld;

};

NAMESPACE_END
