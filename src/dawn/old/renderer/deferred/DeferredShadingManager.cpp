/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "DeferredShadingManager.h"
#include "DeferredLightCompPass.h"
#include "GBufferSchemeHandler.h"
#include "OgreCompositorLogic.h"

namespace dw {

const Ogre::uint8 DeferredShadingManager::PRE_GBUFFER_RENDER_QUEUE = Ogre::RENDER_QUEUE_1;
const Ogre::uint8 DeferredShadingManager::POST_GBUFFER_RENDER_QUEUE = Ogre::RENDER_QUEUE_8;

class DW_API CustomCameraSetup : public Ogre::ShadowCameraSetup {
public:
    CustomCameraSetup() {
    }
    virtual ~CustomCameraSetup() {
    }

    void getShadowCamera(const Ogre::SceneManager* sm, const Ogre::Camera* cam,
                         const Ogre::Viewport* vp, const Ogre::Light* light, Ogre::Camera* texCam,
                         size_t iteration) const override {
        Ogre::Vector3 pos, dir;

        // reset custom view / projection matrix in case already set
        texCam->setCustomViewMatrix(false);
        texCam->setCustomProjectionMatrix(false);
        texCam->setNearClipDistance(light->_deriveShadowNearClipDistance(cam));

        // get the shadow frustum's far distance
        float shadowDist = light->getShadowFarDistance();
        if (!shadowDist) {
            // need a shadow distance, make one up
            shadowDist = cam->getNearClipDistance() * 300;
        }

        float shadowOffset = shadowDist * (sm->getShadowDirLightTextureOffset());
        texCam->setFarClipDistance(sm->getShadowDirectionalLightExtrusionDistance() * 2.0f);

        // Directional lights
        if (light->getType() == Ogre::Light::LT_DIRECTIONAL) {
            // set up the shadow texture
            // Set ortho projection
            texCam->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
            // set ortho window so that texture covers far dist
            texCam->setOrthoWindow(shadowDist * 2, shadowDist * 2);

            // Calculate look at position
            // We want to look at a spot shadowOffset away from near plane
            // 0.5 is a little too close for angles
            Ogre::Vector3 target = cam->getDerivedPosition();  // + (cam->getDerivedDirection() *
                                                               // shadowOffset);

            // Calculate direction, which same as directional light direction
            dir = -light->getDerivedDirection();  // backwards since point down -z
            dir.normalise();

            // Calculate position
            // We want to be in the -ve direction of the light direction
            // far enough to project for the dir light extrusion distance
            pos = target + dir * sm->getShadowDirectionalLightExtrusionDistance();

            // Round local x/y position based on a world-space texel; this helps to
            // reduce
            // jittering caused by the projection moving with the camera
            // Viewport is 2 * near clip distance across (90 degree fov)
            //~ Real worldTexelSize = (texCam->getNearClipDistance() * 20) /
            // vp->getActualWidth();
            //~ pos.x -= fmod(pos.x, worldTexelSize);
            //~ pos.y -= fmod(pos.y, worldTexelSize);
            //~ pos.z -= fmod(pos.z, worldTexelSize);
            float worldTexelSize = (shadowDist * 2) / texCam->getViewport()->getActualWidth();

            // get texCam orientation
            Ogre::Vector3 up = Ogre::Vector3::UNIT_Y;

            // Check it's not coincident with dir
            if (math::Abs(up.dotProduct(dir)) >= 1.0f) {
                // Use camera up
                up = Ogre::Vector3::UNIT_Z;
            }

            // cross twice to re-derive, only direction is unaltered
            Ogre::Vector3 left = dir.crossProduct(up);
            left.normalise();
            up = dir.crossProduct(left);
            up.normalise();
            // Derive quaternion from axes
            Ogre::Quaternion q;
            q.FromAxes(left, up, dir);

            // convert world space camera position into light space
            Ogre::Vector3 lightSpacePos = q.Inverse() * pos;

            // snap to nearest texel
            lightSpacePos.x -= fmod(lightSpacePos.x, worldTexelSize);
            lightSpacePos.y -= fmod(lightSpacePos.y, worldTexelSize);

            // convert back to world space
            pos = q * lightSpacePos;
        }
        // Spotlight
        else if (light->getType() == Ogre::Light::LT_SPOTLIGHT) {
            // Set perspective projection
            texCam->setProjectionType(Ogre::PT_PERSPECTIVE);

            // set FOV slightly larger than the spotlight range to ensure coverage
            Ogre::Radian fovy = light->getSpotlightOuterAngle() * 1.2f;

            // limit angle
            if (fovy.valueDegrees() > 175) {
                fovy = Ogre::Degree(175);
            }

            texCam->setFOVy(fovy);

            // Calculate position, which same as spotlight position
            pos = light->getDerivedPosition();

            // Calculate direction, which same as spotlight direction
            dir = -light->getDerivedDirection();  // backwards since point down -z
            dir.normalise();
        }
        // Point light
        else {
            // Set perspective projection
            texCam->setProjectionType(Ogre::PT_PERSPECTIVE);

            // Use 120 degree FOV for point light to ensure coverage more area
            texCam->setFOVy(Ogre::Degree(120));

            // Calculate look at position
            // We want to look at a spot shadowOffset away from near plane
            // 0.5 is a little too close for angles
            Ogre::Vector3 target =
                cam->getDerivedPosition() + (cam->getDerivedDirection() * shadowOffset);

            // Calculate position, which same as point light position
            pos = light->getDerivedPosition();

            dir = (pos - target);  // backwards since point down -z
            dir.normalise();
        }

        // Finally set position
        texCam->setPosition(pos);

        // Calculate orientation based on direction calculated above
        Ogre::Vector3 up = Ogre::Vector3::UNIT_Y;

        // Check it's not coincident with dir
        if (math::Abs(up.dotProduct(dir)) >= 1.0f) {
            // Use camera up
            up = Ogre::Vector3::UNIT_Z;
        }

        // cross twice to re-derive, only direction is unaltered
        Ogre::Vector3 left = dir.crossProduct(up);
        left.normalise();
        up = dir.crossProduct(left);
        up.normalise();

        // Derive quaternion from axes
        Ogre::Quaternion q;
        q.FromAxes(left, up, dir);
        texCam->setOrientation(q);
    }
};

DeferredShadingManager::DeferredShadingManager(Ogre::Viewport* vp, Ogre::SceneManager* sm)
    : mViewport(vp),
      mSceneManager(sm),
      mGBufferInstance(nullptr),
      mLightingInstance(nullptr),
      mDebugInstance(nullptr),
      mActive(false),
      mDebug(false) {
    /*
    sm->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
    sm->setShadowTextureCasterMaterial("DeferredShading/Shadows/Caster");
    sm->setShadowTextureCount(1);
    sm->setShadowCameraSetup(Ogre::ShadowCameraSetupPtr(new CustomCameraSetup));
    sm->setShadowTextureConfig(0, 2048, 2048, Ogre::PF_FLOAT32_R, 0, 2);
    sm->setShadowFarDistance(200.0f);
    sm->setShadowDirectionalLightExtrusionDistance(2000.0f);
    */

    /*
    float height = 1.0f;
    float width = height / ((float)mViewport->getActualWidth() /
    (float)mViewport->getActualHeight());
    Ogre::MaterialPtr renderMaterial =
    Ogre::MaterialManager::getSingleton().create("RttMat",
    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    renderMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(false);
    renderMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("RttTex")->setTexture(mSceneMgr->getShadowTexture(0));
    Ogre::Rectangle2D *mMiniScreen = new Ogre::Rectangle2D(true);
    mMiniScreen->setCorners(1.0f - width, -1.0f + height, 1.0f, -1.0f);
    mMiniScreen->setBoundingBox(Ogre::AxisAlignedBox(-100000.0f *
    Ogre::Vector3::UNIT_SCALE, 100000.0f * Ogre::Vector3::UNIT_SCALE));
    mMiniScreen->setMaterial("RttMat");
    Ogre::SceneNode* math::MiniScreenNode =
    mSceneMgr->getRootSceneNode()->createChildSceneNode("MiniScreenNode");
    math::MiniScreenNode->attachObject(mMiniScreen);
    mMiniScreen->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
    */

    Ogre::CompositorManager& cm = Ogre::CompositorManager::getSingleton();

    // Hook up the compositor logic and scheme handlers
    mGBufferSchemeHandler.reset(new GBufferSchemeHandler);
    mNoGBufferSchemeHandler.reset(new NoGBufferSchemeHandler);
    Ogre::MaterialManager::getSingleton().addListener(mGBufferSchemeHandler.get(), "GBuffer");
    Ogre::MaterialManager::getSingleton().addListener(mNoGBufferSchemeHandler.get(), "NoGBuffer");
    cm.registerCustomCompositionPass("DeferredLight", new DeferredLightCompositionPass);

    // Set up compositors
    mGBufferInstance = cm.addCompositor(mViewport, "DeferredShading/GBuffer");
    mLightingInstance = cm.addCompositor(mViewport, "DeferredShading/ShowLit");
    mDebugInstance = cm.addCompositor(mViewport, "DeferredShading/ShowDebug");

    // Set as active
    // EnableFog(false);
    SetActive(true);
}

DeferredShadingManager::~DeferredShadingManager() {
    Ogre::CompositorChain* chain =
        Ogre::CompositorManager::getSingleton().getCompositorChain(mViewport);
    chain->_removeInstance(mDebugInstance);
    chain->_removeInstance(mLightingInstance);
    chain->_removeInstance(mGBufferInstance);
    Ogre::CompositorManager::getSingleton().removeCompositorChain(mViewport);

    Ogre::MaterialManager::getSingleton().removeListener(mNoGBufferSchemeHandler.get(),
                                                         "NoGBuffer");
    Ogre::MaterialManager::getSingleton().removeListener(mGBufferSchemeHandler.get(), "GBuffer");
}

void DeferredShadingManager::EnableFog(bool fog) {
    /*
    // Prevent unnecessary setups
    if (mFogEnabled == fog)
        return;

    mFogEnabled = fog;

    // Update visible instance
    if (mDebug == false)
    {
        mLightingNoFogInstance->setEnabled(!fog);
        mLightingInstance->setEnabled(fog);
    }*/
}

void DeferredShadingManager::SetDebugMode(bool debug) {
    // Prevent unnecessary setups
    if (mDebug == debug && mLightingInstance->getEnabled() == !mDebug)
        return;

    mLightingInstance->setEnabled(!debug);
    mDebugInstance->setEnabled(debug);
    mDebug = debug;
}

bool DeferredShadingManager::IsDebugMode() const {
    return mDebug;
}

void DeferredShadingManager::SetActive(bool active) {
    if (mActive != active) {
        mActive = active;
        mGBufferInstance->setEnabled(active);
        SetDebugMode(mDebug);
    }
}
}  // namespace dw
