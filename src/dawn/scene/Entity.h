/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

NAMESPACE_BEGIN

class Camera;
class SceneManager;

class DW_API Entity
{
public:
    Entity(SceneManager* sceneMgr);
    Entity(SceneManager* sceneMgr, const Position& position);
    Entity(SceneManager* sceneMgr, const Position& position, const Quat& orientation);
    virtual ~Entity();

    /// Update the position of this entity in world space
    /// @param position The position to place the entity
    virtual void setPosition(const Position& position);

    /// Update the orientation of this entity
    /// @param orient The new orientation represented as a quaternion
    virtual void setOrientation(const Quat& orient);

    /// Get the position of this entity in world space
    /// @return The current position
    virtual const Position& getPosition() const;

    /// Get the orientation of this entity
    /// @return The current position
    virtual const Quat& getOrientation() const;

    /// Update this entity
    /// Note: Returning false will remove this entity from the scene manager and free it's memory
    /// @param dt Delta time
    virtual bool update(float dt) = 0;

    /// Called before rendering
    virtual void preRender(Camera* camera) = 0;

protected:
    SceneManager* mSceneMgr;

    Position mPosition;
    Quat mOrientation;
};

NAMESPACE_END
