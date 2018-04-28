/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/api/Renderer.h"
#include "scene/PhysicsScene.h"
#include "scene/Camera.h"
#include "StarSystem.h"
#include "Star.h"

namespace dw {

// TODO: Lens flares:
// http://www.ogre3d.org/forums/viewtopic.php?f=11&t=65630

StarDesc::StarDesc() : radius(0.0f), spectralClass(SC_G) {
}

Star::Star(Renderer* renderSystem, StarSystem* starSystem, PhysicsWorld* physicsMgr, StarDesc& desc)
    : SystemBody(renderSystem),
      mPhysicsMgr(physicsMgr),
      mDesc(desc),
      mFlareVisibility(0.0f),
      mFlareBillboard(nullptr),
      mGlowBillboard(nullptr),
      mFlareBillboardSet(nullptr),
      mGlowBillboardSet(nullptr),
      mSceneNode(nullptr) {
    assert(desc.radius > 0.0f);

    // Get the correct colour, and create the scene node
    mSceneNode = starSystem->getRootSceneNode()->createChildSceneNode();

    // Create the flare effect
    mFlareBillboardSet = renderSystem->getSceneMgr()->createBillboardSet(1);
    mFlareBillboardSet->setMaterialName("Scene/Star/Flare");
    mFlareBillboardSet->setBounds(Ogre::AxisAlignedBox::BOX_INFINITE, 1000000.0f);
    mSceneNode->attachObject(mFlareBillboardSet);

    // Create the glow effect
    mGlowBillboardSet = renderSystem->getSceneMgr()->createBillboardSet(1);
    mGlowBillboardSet->setMaterialName("Scene/Star/Glow");
    mGlowBillboardSet->setBounds(Ogre::AxisAlignedBox::BOX_INFINITE, 1000000.0f);
    mSceneNode->attachObject(mGlowBillboardSet);

    // Create Glow Billboard
    // Flare Billboard is created in the update step
    mGlowBillboard =
        mGlowBillboardSet->createBillboard(Ogre::Vector3::ZERO, Colour(1.0f, 1.0f, 1.0f));

    // Create light
    mLight = renderSystem->getSceneMgr()->createLight();
    mLight->setType(Ogre::Light::LT_DIRECTIONAL);
    mLight->setDiffuseColour(Colour(1.0f, 1.0f, 1.0f));
}

Star::~Star() {
    mRenderSystem->getSceneMgr()->destroyLight(mLight);
    mRenderSystem->getSceneMgr()->destroyBillboardSet(mGlowBillboardSet);
    mRenderSystem->getSceneMgr()->destroyBillboardSet(mFlareBillboardSet);
    mRenderSystem->getSceneMgr()->destroySceneNode(mSceneNode);
}

Colour Star::getColour() const {
    Colour colour;

    // Pick the colour based on the class
    switch (mDesc.spectralClass) {
        case SC_O:
            // blue
            colour = Colour(0.6f, 0.7f, 1.0f);
            break;

        case SC_B:
            // blue white
            colour = Colour(0.7f, 0.8f, 1.0f);
            break;

        case SC_A:
            // light blue white
            colour = Colour(0.87f, 0.9f, 1.0f);
            break;

        case SC_F:
            // white
            colour = Colour(0.95f, 0.95f, 1.0f);
            break;

        case SC_G:
            // yellow white
            colour = Colour(1.0f, 0.9f, 0.8f);
            break;

        case SC_K:
            // yellow orange
            colour = Colour(1.0f, 0.82f, 0.6f);
            break;

        case SC_M:
            // orange red
            colour = Colour(1.0f, 0.77f, 0.49f);
            break;

        default:
            colour = Colour(1.0f, 1.0f, 1.0f);
            break;
    }

    return colour;
}

void Star::preRender(Camera* camera) {
    Vec3 localPosition = mPosition.toCameraSpace(camera);

    // Calculate light direction and apparent radius
    float distance = math::Max(localPosition.Length(), 1.0f);
    Vec3 lightDirection = localPosition / distance;
    mLight->setDirection(-lightDirection);
    float apparentRadius = mDesc.radius / (math::Tan(camera->getFov() * 0.5f) * distance);

    // Position the billboards
    float scale = 100000.0f;
    mGlowBillboard->setPosition(lightDirection * scale);
    if (mFlareBillboard)
        mFlareBillboard->setPosition(mGlowBillboard->mPosition);

    // Scale the billboards
    mGlowBillboard->setDimensions(apparentRadius * scale * 120.0f, apparentRadius * scale * 120.0f);
    if (mFlareBillboard)
        mFlareBillboard->setDimensions(apparentRadius * scale * 120.0f,
                                       apparentRadius * scale * 60.0f);

    // Perform a ray cast and fade out the flare
    PhysicsRaycastResult query;
    mPhysicsMgr->rayQuery(camera->getPosition(), mPosition, camera, query);
    mFlareVisibility =
        math::Clamp(mFlareVisibility + (query.hit ? -1.0f : 1.0f) * 8.0f / 60.0f, 0.0f, 1.0f);
    if (mFlareVisibility < M_EPSILON && mFlareBillboard) {
        mFlareBillboardSet->removeBillboard(mFlareBillboard);
        mFlareBillboard = nullptr;
    } else if (mFlareVisibility > 0.0f && !mFlareBillboard) {
        mFlareBillboard = mFlareBillboardSet->createBillboard(mGlowBillboard->getPosition());
    }

    if (mFlareBillboard) {
        mFlareBillboard->setColour(getColour() * mFlareVisibility);
    }

    // Update satellites
    SystemBody::preRender(camera);
}
}  // namespace dw
