/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

// Derives from Entity
#include "Entity.h"

NAMESPACE_BEGIN

class SceneManager;

// A camera entity, used as a viewpoint into the world. Derive this class to implement behaviours
class Camera : public Entity
{
public:
    Camera(SceneManager* sceneMgr);
    virtual ~Camera();

    /// Return this cameras view matrix
    Mat4 GetViewMatrix() const { return mCamera->getViewMatrix(); }

    /// Return this cameras projection matrix
    Mat4 GetProjMatrix() const { return mCamera->getProjectionMatrix(); }

    /// Set this cameras field of view in degrees
    /// @param fov New FOV
    void SetFOV(float fov);

    /// Return this cameras field of view in degrees
    /// @returns This cameras FOV
    float GetFOV() const { return mCamera->getFOVy().valueDegrees(); }

    /// Returns the ogre camera associated with this camera
    Ogre::Camera* GetOgreCamera() { return mCamera; }
    Ogre::SceneNode* GetOgreCameraSceneNode() { return mSceneNode; }

    // Inherited from Entity
    virtual void SetOrientation(const Quat& orientation) override;
    virtual bool Update(float dt) override { return true; }
    virtual void PreRender(Camera* camera) override {}

private:
    Ogre::Camera* mCamera;
    Ogre::SceneNode* mSceneNode;

};

NAMESPACE_END
