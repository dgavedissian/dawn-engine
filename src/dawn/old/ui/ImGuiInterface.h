/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

namespace dw {

class Renderer;
class Input;

class ImGuiInterface {
public:
    ImGuiInterface(Renderer* rs, Input* im, Ogre::MaterialPtr uiMaterial,
                   const Ogre::Matrix4& projection);
    ~ImGuiInterface();

    void beginFrame();

    // Input events
    void onMouseButton(int button);
    void onMouseScroll(float scroll);
    void onKey(SDL_Keycode key, Uint16 mod, bool down);
    void onTextInput(const String& s);

private:
    Input* mInputMgr;

    ImGuiIO& mIO;

    Ogre::RenderSystem* mRenderSystem;
    Ogre::SceneManager* mSceneMgr;
    Ogre::RenderOperation mRenderOp;
    Ogre::MaterialPtr mUIMaterial;
    Ogre::Matrix4 mProjection;
    uint mVbSize;
    uint mIbSize;

    uint mWidth;
    uint mHeight;

    bool mMousePressed[3];
    float mMouseWheel;

private:
    void createFontsTexture();
    void allocateVertexBuffer(uint size);
    void allocateIndexBuffer(uint size);
    void renderDrawLists(ImDrawData* drawData);

    // Callback used by ImGui
    static void renderDrawListsCallback(ImDrawData* drawData);
};
}
