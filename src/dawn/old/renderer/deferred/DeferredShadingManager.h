/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

namespace dw {

class GBufferSchemeHandler;
class NoGBufferSchemeHandler;

// System to manage Deferred Shading for a camera/render target.
class DW_API DeferredShadingManager : public Ogre::RenderTargetListener {
public:
    // The first render queue that does get rendered into the GBuffer
    // place objects (like skies) that should be before GBuffer before this one.
    static const Ogre::uint8 PRE_GBUFFER_RENDER_QUEUE;

    // The first render queue that does not get rendered into the GBuffer
    // place transparent (or other non GBuffer) objects after this one
    static const Ogre::uint8 POST_GBUFFER_RENDER_QUEUE;

    DeferredShadingManager(Ogre::Viewport* vp, Ogre::SceneManager* sm);
    ~DeferredShadingManager();

    // Enable fog?
    void EnableFog(bool fog);

    // Set debug mode
    void SetDebugMode(bool debug);

    // Is debug mode
    bool IsDebugMode() const;

    // Activate/Deactivate the system
    void SetActive(bool active);

protected:
    Ogre::Viewport* mViewport;
    Ogre::SceneManager* mSceneManager;

    Ogre::CompositorInstance* mGBufferInstance;
    Ogre::CompositorInstance* mLightingInstance;
    Ogre::CompositorInstance* mLightingNoFogInstance;
    Ogre::CompositorInstance* mDebugInstance;

    SharedPtr<GBufferSchemeHandler> mGBufferSchemeHandler;
    SharedPtr<NoGBufferSchemeHandler> mNoGBufferSchemeHandler;

    bool mActive;
    bool mFogEnabled;
    bool mDebug;
};
}  // namespace dw
