/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#include "Common.h"
#include "Graphics/RenderSystem.h"
#include "Input/InputManager.h"
#include "ImGuiInterface.h"

NAMESPACE_BEGIN

ImGuiInterface* gCurrentImGuiInterface = nullptr;

// Wraps an Ogre TexturePtr so the reference count doesn't go to 0
struct ImguiTextureHandle
{
    Ogre::TexturePtr texturePtr;
};

ImGuiInterface::ImGuiInterface(RenderSystem* rs, InputManager* im)
    : mInputMgr(im),
      mIO(ImGui::GetIO()),
      mRenderSystem(rs->GetOgreRenderSystem()),
      mWidth(rs->GetWidth()),
      mHeight(rs->GetHeight())
{
    assert(gCurrentImGuiInterface == nullptr);
    gCurrentImGuiInterface = this;

    mIO.DisplaySize.x = (float)mWidth;
    mIO.DisplaySize.y = (float)mHeight;
    mIO.RenderDrawListsFn = ImGuiInterface::RenderDrawListsCallback;

    // Set up font
    mIO.Fonts->AddFontDefault();
    CreateFontsTexture();

    // Set up key bindings
    mIO.KeyMap[ImGuiKey_Tab] = SDLK_TAB;
    mIO.KeyMap[ImGuiKey_LeftArrow] = SDLK_LEFT;
    mIO.KeyMap[ImGuiKey_RightArrow] = SDLK_RIGHT;
    mIO.KeyMap[ImGuiKey_UpArrow] = SDLK_UP;
    mIO.KeyMap[ImGuiKey_DownArrow] = SDLK_DOWN;
    mIO.KeyMap[ImGuiKey_Home] = SDLK_HOME;
    mIO.KeyMap[ImGuiKey_End] = SDLK_END;
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
    mRenderOp.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;
    mRenderOp.useIndexes = false;

    // Set up vertex declaration
    Ogre::VertexDeclaration* vd = mRenderOp.vertexData->vertexDeclaration;
    size_t offset = 0;
    vd->addElement(0, offset, Ogre::VET_FLOAT2, Ogre::VES_POSITION);
    offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT2);
    vd->addElement(0, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
    offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT2);
    vd->addElement(0, offset, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);

    // Create initial vertex buffer
    mSize = 1000;
    AllocateVertexBuffer(mSize);

    // Configure the alpha blending mode
    mAlphaBlendMode.blendType = Ogre::LBT_ALPHA;
    mAlphaBlendMode.source1 = Ogre::LBS_DIFFUSE;
    mAlphaBlendMode.source2 = Ogre::LBS_TEXTURE;
    mAlphaBlendMode.operation = Ogre::LBX_MODULATE;
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

    mIO.KeysDown[key] = down;
    mIO.KeyCtrl = mod & KMOD_CTRL;
    mIO.KeyShift = mod & KMOD_SHIFT;
    mIO.KeyAlt = mod & KMOD_ALT;
}

void ImGuiInterface::OnTextInput(char c)
{
    mIO.AddInputCharacter((unsigned short)c);
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
            mRenderOp.vertexData->vertexDeclaration->getVertexSize(0), mSize, 
            Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
    mRenderOp.vertexData->vertexBufferBinding->setBinding(0, vb);
}

void ImGuiInterface::RenderDrawLists(ImDrawList** const cmdLists, int cmdListsCount)
{
    // Assume render system is configured according to InterfaceManager::_configureRenderSystem
    mRenderSystem->_setWorldMatrix(Ogre::Matrix4::IDENTITY);
    
    // For each command list
    for (int i = 0; i < cmdListsCount; i++)
    {
        ImVector<ImDrawCmd>& commands = cmdLists[i]->commands;

        // Allocate more space if needed
        if (mSize < cmdLists[i]->vtx_buffer.size())
        {
            mSize = cmdLists[i]->vtx_buffer.size();
            AllocateVertexBuffer(mSize);
        }

        // Copy vertices into VB
        Ogre::HardwareVertexBufferSharedPtr vb =
            mRenderOp.vertexData->vertexBufferBinding->getBuffer(0);
        ImDrawVert* vertices = static_cast<ImDrawVert*>(
            vb->lock(0, vb->getSizeInBytes(), Ogre::HardwareBuffer::HBL_DISCARD));
        memcpy(vertices, &cmdLists[i]->vtx_buffer[0], cmdLists[i]->vtx_buffer.size() * sizeof(ImDrawVert));
        vb->unlock();

        // Execute draw calls
        int offset = 0;
        for (auto c : commands)
        {
            mRenderSystem->setScissorTest(true, c.clip_rect.x, c.clip_rect.y, c.clip_rect.z, c.clip_rect.w);

            // Set texture
            if (c.texture_id)
            {
                ImguiTextureHandle* handle = reinterpret_cast<ImguiTextureHandle*>(c.texture_id);
                mRenderSystem->_setTexture(0, true, handle->texturePtr);
                mRenderSystem->_setTextureBlendMode(0, mAlphaBlendMode);
            }
            else
            {
                mRenderSystem->_disableTextureUnit(0);
            }

            // Draw vertices
            mRenderOp.vertexData->vertexStart = offset;
            mRenderOp.vertexData->vertexCount = c.vtx_count;
            mRenderSystem->_render(mRenderOp);
            offset += c.vtx_count;
        }
    }
    mRenderSystem->setScissorTest(false);
}

void ImGuiInterface::RenderDrawListsCallback(ImDrawList** const cmdLists, int cmdListsCount)
{
    gCurrentImGuiInterface->RenderDrawLists(cmdLists, cmdListsCount);
}

NAMESPACE_END
