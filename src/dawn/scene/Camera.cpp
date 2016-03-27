/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "core/Utils.h"
#include "scene/Entity.h"
#include "scene/SceneManager.h"
#include "scene/Camera.h"

NAMESPACE_BEGIN

Camera::Camera(SceneManager* sceneMgr)
    : Entity(sceneMgr),
      mCamera(nullptr),
      mSceneNode(nullptr)
{
    mCamera = sceneMgr->mSceneMgr->createCamera(generateName());

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

void Camera::setFov(float fov)
{
    mCamera->setFOVy(Ogre::Degree(fov));
}

void Camera::setOrientation(const Quat& orientation)
{
    Entity::setOrientation(orientation);
    mSceneNode->setOrientation(orientation);
}

NAMESPACE_END
