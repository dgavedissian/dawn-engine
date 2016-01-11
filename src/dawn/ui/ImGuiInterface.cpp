/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#include "Common.h"
#include "renderer/Renderer.h"
#include "input/Input.h"
#include "ImGuiInterface.h"

NAMESPACE_BEGIN

ImGuiInterface* gCurrentImGuiInterface = nullptr;

// Wraps an Ogre TexturePtr so the reference count doesn't go to 0
struct ImguiTextureHandle
{
    Ogre::TexturePtr texturePtr;
};

ImGuiInterface::ImGuiInterface(Renderer* rs, Input* im, Ogre::MaterialPtr uiMaterial,
                               const Ogre::Matrix4& projection)
    : mInputMgr(im),
      mIO(ImGui::GetIO()),
      mRenderSystem(rs->GetOgreRenderSystem()),
      mSceneMgr(rs->GetSceneMgr()),
      mUIMaterial(uiMaterial),
      mProjection(projection),
      mVbSize(1000),
      mIbSize(1000),
      mWidth(rs->GetWidth()),
      mHeight(rs->GetHeight())
{
    assert(gCurrentImGuiInterface == nullptr);
    gCurrentImGuiInterface = this;

    mIO.DisplaySize.x = (float)mWidth;
    mIO.DisplaySize.y = (float)mHeight;
    mIO.IniFilename = NULL;
    mIO.LogFilename = NULL;
    mIO.RenderDrawListsFn = ImGuiInterface::RenderDrawListsCallback;

    // Set up font
    mIO.Fonts->AddFontDefault();
    CreateFontsTexture();

    // Set up key bindings
    mIO.KeyMap[ImGuiKey_Tab] = SDLK_TAB;
    mIO.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
    mIO.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
    mIO.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
    mIO.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
    mIO.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
    mIO.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
    mIO.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
    mIO.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
    mIO.KeyMap[ImGuiKey_Delete] = SDLK_DELETE;
    mIO.KeyMap[ImGuiKey_Backspace] = SDLK_BACKSPACE;
    mIO.KeyMap[ImGuiKey_Enter] = SDLK_RETURN;
    mIO.KeyMap[ImGuiKey_Escape] = SDLK_ESCAPE;
    mIO.KeyMap[ImGuiKey_A] = SDLK_a;
    mIO.KeyMap[ImGuiKey_C] = SDLK_c;
    mIO.KeyMap[ImGuiKey_V] = SDLK_v;
    mIO.KeyMap[ImGuiKey_X] = SDLK_x;
    mIO.KeyMap[ImGuiKey_Y] = SDLK_y;
    mIO.KeyMap[ImGuiKey_Z] = SDLK_z;

    // Set up vertex data
    mRenderOp.vertexData = new Ogre::VertexData();
    mRenderOp.vertexData->vertexStart = 0;
    mRenderOp.vertexData->vertexCount = mVbSize;
    mRenderOp.indexData = new Ogre::IndexData();
    mRenderOp.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;
    mRenderOp.useIndexes = true;

    // Set up vertex declaration
    Ogre::VertexDeclaration* vd = mRenderOp.vertexData->vertexDeclaration;
    size_t offset = 0;
    vd->addElement(0, offset, Ogre::VET_FLOAT2, Ogre::VES_POSITION);
    offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT2);
    vd->addElement(0, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
    offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT2);
    vd->addElement(0, offset, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);

    // Create initial buffers
    AllocateVertexBuffer(mVbSize);
    AllocateIndexBuffer(mIbSize);
}

ImGuiInterface::~ImGuiInterface()
{
    gCurrentImGuiInterface = nullptr;
}

void ImGuiInterface::BeginFrame()
{
    // Copy mouse position
    if (true) // TODO: This window is focused
    {
        Vec2i mp = mInputMgr->GetMousePosition();
        mIO.MousePos.x = mp.x;
        mIO.MousePos.y = mp.y;
    }
    else
    {
    	mIO.MousePos = ImVec2(-1,-1);
    }

    // Copy mouse state
    for (int i = 0; i < 3; i++)
    {
        // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss
        // click-release events that are shorter than 1 frame.
        mIO.MouseDown[i] = mMousePressed[i] || mInputMgr->IsMouseButtonDown(SDL_BUTTON_LEFT + i);
        mMousePressed[i] = false;
    }

    // Mouse moved since last frame
    mIO.MouseWheel = mMouseWheel;
    mMouseWheel = 0.0f;

    ImGui::NewFrame();
}

void ImGuiInterface::OnMouseButton(int button)
{
    button -= SDL_BUTTON_LEFT;
    if (button >= 0 && button < 3)
        mMousePressed[button] = true;
}

void ImGuiInterface::OnMouseScroll(float scroll)
{
    mMouseWheel += scroll;
}

void ImGuiInterface::OnKey(SDL_Keycode key, Uint16 mod, bool down)
{
    if (key < 0x0 || key > 0x200)
        return;

    mIO.KeysDown[key & ~SDLK_SCANCODE_MASK] = down;
    mIO.KeyCtrl = mod & KMOD_CTRL;
    mIO.KeyShift = mod & KMOD_SHIFT;
    mIO.KeyAlt = mod & KMOD_ALT;
}

void ImGuiInterface::OnTextInput(const String& s)
{
    mIO.AddInputCharactersUTF8(s.c_str());
}

void ImGuiInterface::CreateFontsTexture()
{
    unsigned char* pixels;
    int width, height;
    mIO.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    // Create texture from data stream
    Ogre::DataStreamPtr stream(OGRE_NEW Ogre::MemoryDataStream(
        (void*)pixels, width * height * sizeof(uint)));
    Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().loadRawData(
        "ImGuiFont", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, stream,
        (Ogre::ushort)width, (Ogre::ushort)height, Ogre::PF_A8B8G8R8, Ogre::TEX_TYPE_2D, 0);

    ImguiTextureHandle* handle = new ImguiTextureHandle;
    handle->texturePtr = texture;
    mIO.Fonts->TexID = reinterpret_cast<void*>(handle);
}

void ImGuiInterface::AllocateVertexBuffer(uint size)
{
    Ogre::HardwareVertexBufferSharedPtr vb =
        Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
            mRenderOp.vertexData->vertexDeclaration->getVertexSize(0), mVbSize,
            Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
    mRenderOp.vertexData->vertexBufferBinding->setBinding(0, vb);
    mRenderOp.vertexData->vertexCount = mVbSize;
}

void ImGuiInterface::AllocateIndexBuffer(uint size)
{
    Ogre::HardwareIndexBufferSharedPtr ib =
        Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
            Ogre::HardwareIndexBuffer::IT_16BIT, size,
            Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
    mRenderOp.indexData->indexBuffer = ib;
}

void ImGuiInterface::RenderDrawLists(ImDrawData* drawData)
{
    // For each command List
    for (int i = 0; i < drawData->CmdListsCount; i++)
    {
        auto cmdList = drawData->CmdLists[i];
        ImVector<ImDrawCmd>& commands = cmdList->CmdBuffer;

        // Expand the vertex buffer
        if (mVbSize < cmdList->VtxBuffer.size())
        {
            mVbSize = cmdList->VtxBuffer.size();
            AllocateVertexBuffer(mVbSize);
        }

        // Fill the vertex buffer
        Ogre::HardwareVertexBufferSharedPtr vb = mRenderOp.vertexData->vertexBufferBinding->getBuffer(0);
        void* vertexData = vb->lock(0, vb->getSizeInBytes(), Ogre::HardwareBuffer::HBL_DISCARD);
        memcpy(vertexData, &cmdList->VtxBuffer.front(), cmdList->VtxBuffer.size() * sizeof(ImDrawVert));
        vb->unlock();

        // Expand the index buffer
        if (mIbSize < cmdList->IdxBuffer.size())
        {
            mIbSize = cmdList->IdxBuffer.size();
            AllocateIndexBuffer(mIbSize);
        }

        // Fill the index buffer
        Ogre::HardwareIndexBufferSharedPtr ib = mRenderOp.indexData->indexBuffer;
        void* indexData = ib->lock(0, ib->getSizeInBytes(), Ogre::HardwareBuffer::HBL_DISCARD);
        memcpy(indexData, &cmdList->IdxBuffer.front(), cmdList->IdxBuffer.size() * sizeof(ImDrawIdx));
        ib->unlock();

        // Execute draw calls
        int offset = 0;
        for (auto c : commands)
        {
            // Set up pass
            Ogre::Pass* pass;
            ImguiTextureHandle* handle = reinterpret_cast<ImguiTextureHandle*>(c.TextureId);
            if (handle)
            {
                pass = mUIMaterial->getTechnique("Texture")->getPass(0);
                pass->getTextureUnitState(0)->setTexture(handle->texturePtr);
            }
            else
            {
                pass = mUIMaterial->getTechnique("NoTexture")->getPass(0);
            }

            // Draw vertices
            mRenderSystem->setScissorTest(true, c.ClipRect.x, c.ClipRect.y, c.ClipRect.z, c.ClipRect.w);
            mRenderOp.indexData->indexStart = offset;
            mRenderOp.indexData->indexCount = c.ElemCount;
            mSceneMgr->manualRender(&mRenderOp, pass, nullptr,
                                    Ogre::Matrix4::IDENTITY, Ogre::Matrix4::IDENTITY, mProjection);
            offset += c.ElemCount;
        }
    }
    mRenderSystem->setScissorTest(false);
}

void ImGuiInterface::RenderDrawListsCallback(ImDrawData* drawData)
{
    gCurrentImGuiInterface->RenderDrawLists(drawData);
}

NAMESPACE_END
