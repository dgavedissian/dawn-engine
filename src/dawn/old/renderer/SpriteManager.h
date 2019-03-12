/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#pragma once

#include "core/mathRect.h"

namespace dw {

enum SpriteOrigin { SO_TOP_LEFT, SO_TOP_RIGHT, SO_BOTTOM_LEFT, SO_BOTTOM_RIGHT, SO_CENTRE };

struct SpriteChunk {
    struct Vertex {
        Vec2 p;
        Colour c;
        Vec2 tc;
    } vertex[4];
};

struct VertexChunk {
    Ogre::Pass* pass;
    uint vertexCount;
};

struct SpriteDesc {
    String textureName;
    Vec2 position, size, origin;
    Rect uv;
    Colour colour;
    float angle;
};

class DW_API SpriteManager : public Ogre::RenderQueueListener {
private:
public:
    SpriteManager(Ogre::Viewport* viewport, Ogre::SceneManager* sceneMgr);
    virtual ~SpriteManager();

    // Caches a material
    void loadSprite(const String& textureName);

    // Buffers a sprite to be sent to the screen at render time
    void drawSprite(const String& textureName, const Vec2& position,
                    SpriteOrigin origin = SO_TOP_LEFT, float angle = 0.0f);
    void drawSprite(const String& textureName, const Vec2& position, const Vec2& origin,
                    float angle = 0.0f);
    void drawSprite(const String& textureName, const Vec2& position, const Vec2& size,
                    const Vec2& origin, float angle = 0.0f);
    void drawSprite(const SpriteDesc& s);

    // Inherited from Ogre::RenderQueueListener
    virtual void renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation,
                                  bool& repeatThisInvocation) override;

private:
    // Render all the 2D data stored in the hardware buffers.
    void render();

    // Create a new hardware buffer
    void createHardwareBuffer(uint size);

    // Destroy the hardware buffer
    void destroyHardwareBuffer();

    // Rotate a point about (0, 0)
    Vec2 rotate(const Vec2& c, float angle);

    // Convert a position to screen coordinates
    Vec2 toScreenCoord(const Vec2& pos);

    // Ogre specifics
    Ogre::RenderSystem* mRenderSystem;
    Ogre::Viewport* mViewport;
    Ogre::SceneManager* mSceneManager;
    Ogre::RenderOperation mRenderOp;
    Ogre::HardwareVertexBufferSharedPtr mHwBuffer;

    // Sprites - organised by Texture Name
    uint mSpriteCount;
    Map<String, Vector<SpriteChunk>> mChunks;
};
}  // namespace dw
