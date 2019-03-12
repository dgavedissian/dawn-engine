/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "../GeometryUtils.h"
#include "DeferredLightCompPass.h"
#include "LightMaterialGenerator.h"

namespace dw {

DeferredLightRenderOperation::DeferredLightRenderOperation(Ogre::CompositorInstance* instance,
                                                           const Ogre::CompositionPass* pass) {
    mViewport = instance->getChain()->getViewport();

    // Create lights material generator
    mLightMaterialGenerator = new LightMaterialGenerator();

    // Create the ambient light
    mAmbientLight = new AmbientLight();
    const Ogre::MaterialPtr& mat = mAmbientLight->getMaterial();
    mat->load();

    // Set up DeferredLight render operations

    // Directional Light
    mDirectionalLightOp.operationType = Ogre::RenderOperation::OT_TRIANGLE_STRIP;
    mDirectionalLightOp.vertexData = new Ogre::VertexData();
    mDirectionalLightOp.indexData = nullptr;
    mDirectionalLightOp.useIndexes = false;
    createQuad(mDirectionalLightOp.vertexData);

    // Point Light
    mPointLightOp.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;
    mPointLightOp.vertexData = new Ogre::VertexData();
    mPointLightOp.indexData = new Ogre::IndexData();
    mPointLightOp.useIndexes = true;
    createSphere(mPointLightOp.vertexData, mPointLightOp.indexData, 1.0f, 10, 10, false, false);

    // Spot Light
    mSpotlightOp.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;
    mSpotlightOp.vertexData = new Ogre::VertexData();
    mSpotlightOp.indexData = new Ogre::IndexData();
    mSpotlightOp.useIndexes = true;
    createCone(mSpotlightOp.vertexData, mSpotlightOp.indexData, 1.0f, 1.0f, 20);
}

DeferredLightRenderOperation::~DeferredLightRenderOperation() {
    for (LightsMap::iterator it = mLights.begin(); it != mLights.end(); ++it)
        delete it->second;

    mLights.clear();

    delete mSpotlightOp.indexData;
    delete mSpotlightOp.vertexData;
    delete mPointLightOp.indexData;
    delete mPointLightOp.vertexData;
    delete mDirectionalLightOp.vertexData;

    delete mAmbientLight;
    delete mLightMaterialGenerator;
}

DeferredLight* DeferredLightRenderOperation::CreateDeferredLight(Ogre::Light* light) {
    DeferredLight* rv = new DeferredLight(mLightMaterialGenerator, light, this);
    mLights[light] = rv;
    return rv;
}

void injectTechnique(Ogre::SceneManager* sm, Ogre::Technique* tech, Ogre::Renderable* rend,
                     const Ogre::LightList* lightList) {
    for (unsigned short i = 0; i < tech->getNumPasses(); ++i) {
        Ogre::Pass* pass = tech->getPass(i);
        if (lightList != nullptr)
            sm->_injectRenderWithPass(pass, rend, false, false, lightList);
        else
            sm->_injectRenderWithPass(pass, rend, false);
    }
}

void DeferredLightRenderOperation::execute(Ogre::SceneManager* sm, Ogre::RenderSystem* rs) {
    Ogre::Camera* cam = mViewport->getCamera();

    mAmbientLight->updateFromCamera(cam);
    Ogre::Technique* tech = mAmbientLight->getMaterial()->getBestTechnique();
    injectTechnique(sm, tech, mAmbientLight, nullptr);

    const Ogre::LightList& lightList = sm->_getLightsAffectingFrustum();
    for (Ogre::LightList::const_iterator i = lightList.begin(); i != lightList.end(); ++i) {
        Ogre::Light* light = *i;
        Ogre::LightList ll;
        ll.push_back(light);

        LightsMap::iterator dLightIt = mLights.find(light);
        DeferredLight* dLight = nullptr;

        if (dLightIt == mLights.end()) {
            dLight = CreateDeferredLight(light);
        } else {
            dLight = dLightIt->second;
            dLight->updateFromParent();
        }

        dLight->updateFromCamera(cam);
        tech = dLight->getMaterial()->getBestTechnique();

        // Update shadow texture
        if (dLight->getCastChadows()) {
            Ogre::SceneManager::RenderContext* context = sm->_pauseRendering();

            sm->prepareShadowTextures(cam, mViewport, &ll);
            sm->_resumeRendering(context);

            Ogre::TextureUnitState* tus = tech->getPass(0)->getTextureUnitState("ShadowMap");
            assert(tus);
            const Ogre::TexturePtr& shadowTex = sm->getShadowTexture(0);

            if (tus->_getTexturePtr() != shadowTex)
                tus->_setTexturePtr(shadowTex);
        }

        injectTechnique(sm, tech, dLight, &ll);
    }
}
}  // namespace dw
