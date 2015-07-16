/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

NAMESPACE_BEGIN

class Camera;
class SceneManager;

class DW_API Entity : public EnableSharedFromThis<Entity> // TODO: remove, required in Transcendent
{
public:
    Entity(SceneManager* sceneMgr);
    Entity(SceneManager* sceneMgr, const Position& position);
    Entity(SceneManager* sceneMgr, const Position& position, const Quat& orientation);
    virtual ~Entity();

    /// Update the position of this entity in world space
    /// @param position The position to place the entity
    virtual void SetPosition(const Position& position);

    /// Update the orientation of this entity
    /// @param orient The new orientation represented as a quaternion
    virtual void SetOrientation(const Quat& orient);

    /// Get the position of this entity in world space
    /// @return The current position
    virtual const Position& GetPosition() const;

    /// Get the orientation of this entity
    /// @return The current position
    virtual const Quat& GetOrientation() const;

    /// Update this entity
    /// Note: Returning false will remove this entity from the scene manager and free it's memory
    /// @param dt Delta time
    virtual bool Update(float dt) = 0;

    /// Called before rendering
    virtual void PreRender(Camera* camera) = 0;

protected:
    SceneManager* mSceneMgr;

    Position mPosition;
    Quat mOrientation;
};

NAMESPACE_END
