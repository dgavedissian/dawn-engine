/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "OgreCompositorInstance.h"
#include "OgreCustomCompositionPass.h"

#include "DeferredLight.h"
#include "MaterialGenerator.h"
#include "AmbientLight.h"

namespace dw {

// The render operation that will be called each frame in the custom composition pass. This is the
// class that will send the actual render calls of the spheres (point lights), cones (spotlights)
// and quads (directional lights) after the GBuffer has been constructed
class DW_API DeferredLightRenderOperation : public Ogre::CompositorInstance::RenderSystemOperation {
public:
    friend class DW_API DeferredLight;

    DeferredLightRenderOperation(Ogre::CompositorInstance* instance,
                                 const Ogre::CompositionPass* pass);
    virtual ~DeferredLightRenderOperation();

    // Inherited from Ogre::CompositorInstance::RenderSystemOperation
    virtual void execute(Ogre::SceneManager* sm, Ogre::RenderSystem* rs) override;

private:
    // Create a new deferred light
    DeferredLight* CreateDeferredLight(Ogre::Light* light);

    // The material generator for the light geometry
    MaterialGenerator* mLightMaterialGenerator;

    // The map of deferred light geometries already Constructed
    typedef Map<Ogre::Light*, DeferredLight*> LightsMap;
    LightsMap mLights;

    // The ambient light used to render the scene
    AmbientLight* mAmbientLight;

    // The viewport that we are rendering to
    Ogre::Viewport* mViewport;

    // Render operations for different light types
    // The mesh data stored in point and spot lights are unit sized
    Ogre::RenderOperation mDirectionalLightOp;
    Ogre::RenderOperation mPointLightOp;
    Ogre::RenderOperation mSpotlightOp;
};

// The custom composition pass that is used for rendering the light geometry
// This class needs to be registered with the CompositorManager
class DW_API DeferredLightCompositionPass : public Ogre::CustomCompositionPass {
public:
    virtual Ogre::CompositorInstance::RenderSystemOperation* createOperation(
        Ogre::CompositorInstance* instance, const Ogre::CompositionPass* pass) override {
        return OGRE_NEW DeferredLightRenderOperation(instance, pass);
    }

protected:
    virtual ~DeferredLightCompositionPass() {
    }
};
}  // namespace dw
