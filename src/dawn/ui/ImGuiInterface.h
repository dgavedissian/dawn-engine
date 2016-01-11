/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

NAMESPACE_BEGIN

class Renderer;
class Input;

class ImGuiInterface
{
public:
    ImGuiInterface(Renderer* rs, Input* im, Ogre::MaterialPtr uiMaterial,
                   const Ogre::Matrix4& projection);
    ~ImGuiInterface();

    void BeginFrame();

    // Input events
    void OnMouseButton(int button);
    void OnMouseScroll(float scroll);
    void OnKey(SDL_Keycode key, Uint16 mod, bool down);
    void OnTextInput(const String& s);

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
    void CreateFontsTexture();
    void AllocateVertexBuffer(uint size);
    void AllocateIndexBuffer(uint size);
    void RenderDrawLists(ImDrawData* drawData);

    // Callback used by ImGui
    static void RenderDrawListsCallback(ImDrawData* drawData);
};

NAMESPACE_END

