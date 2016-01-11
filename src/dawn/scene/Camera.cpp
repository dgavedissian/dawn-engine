/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#include "Common.h"
#include "Entity.h"
#include "SceneManager.h"
#include "Camera.h"

NAMESPACE_BEGIN

Camera::Camera(SceneManager* sceneMgr)
    : Entity(sceneMgr),
      mCamera(nullptr),
      mSceneNode(nullptr)
{
    mCamera = sceneMgr->mSceneMgr->createCamera(GenerateName());

    // Create the camera and attach it to the viewport
    mSceneNode = sceneMgr->mSceneMgr->getRootSceneNode()->createChildSceneNode();
    mSceneNode->attachObject(mCamera);

    // Set up the camera
    mCamera->setNearClipDistance(1.0f);
    mCamera->setFarClipDistance(1000000000.0f);
}

Camera::~Camera()
{
    mSceneMgr->mSceneMgr->destroyCamera(mCamera);
}

void Camera::SetFOV(float fov)
{
    mCamera->setFOVy(Ogre::Degree(fov));
}

void Camera::SetOrientation(const Quat& orientation)
{
    Entity::SetOrientation(orientation);
    mSceneNode->setOrientation(orientation);
}

NAMESPACE_END
