/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

// Derives from Entity
#include "Entity.h"

NAMESPACE_BEGIN

class SceneManager;

// A camera entity, used as a viewpoint into the world. Derive this class to implement behaviours
class DW_API Camera : public Entity
{
public:
    Camera(SceneManager* sceneMgr);
    virtual ~Camera();

    /// Return this cameras view matrix
    Mat4 getViewMatrix() const { return mCamera->getViewMatrix(); }

    /// Return this cameras projection matrix
    Mat4 getProjMatrix() const { return mCamera->getProjectionMatrix(); }

    /// Set this cameras field of view in degrees
    /// @param fov New FOV
    void setFov(float fov);

    /// Return this cameras field of view in degrees
    /// @returns This cameras FOV
    float getFov() const { return mCamera->getFOVy().valueDegrees(); }

    /// Returns the ogre camera associated with this camera
    DEPRECATED Ogre::Camera* _getOgreCamera() { return mCamera; }
    DEPRECATED Ogre::SceneNode* _getOgreSceneNode() { return mSceneNode; }

    // Inherited from Entity
    virtual void setOrientation(const Quat& orientation) override;
    virtual bool update(float dt) override { return true; }
    virtual void preRender(Camera* camera) override {}

private:
    Ogre::Camera* mCamera;
    Ogre::SceneNode* mSceneNode;

};

NAMESPACE_END
