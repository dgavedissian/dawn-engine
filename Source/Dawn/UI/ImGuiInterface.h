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
    ImGuiInterface(Renderer* rs, Input* im);
    ~ImGuiInterface();

    void BeginFrame();

    // Input events
    void OnMouseButton(int button);
    void OnMouseScroll(float scroll);
    void OnKey(SDL_Keycode key, Uint16 mod, bool down);
    void OnTextInput(char c);

private:
    Input* mInputMgr;

    ImGuiIO& mIO;

    Ogre::RenderSystem* mRenderSystem;
    Ogre::RenderOperation mRenderOp;
    Ogre::LayerBlendModeEx mAlphaBlendMode;
    uint mSize;

    uint mWidth;
    uint mHeight;

    bool mMousePressed[3];
    float mMouseWheel;

    void CreateFontsTexture();
    void AllocateVertexBuffer(uint size);
    void RenderDrawLists(ImDrawList** const cmdLists, int cmdListsCount);
    
    // Callback used by ImGui
    static void RenderDrawListsCallback(ImDrawList** const cmdLists, int cmdListsCount);
};

NAMESPACE_END

