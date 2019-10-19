/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "core/Utils.h"
#include "renderer/api/Renderer.h"
#include "ui/RocketInterface.h"

namespace dw {

// The structure created for each texture loaded by Rocket for Ogre
struct RocketOgreTexture {
    RocketOgreTexture(Ogre::TexturePtr _texture) : texture(_texture) {
    }
    Ogre::TexturePtr texture;
};

// The structure created for each set of geometry that Rocket compiles. It stores the vertex and
// index buffers and the texture associated with the geometry, if one was specified
struct RocketOgreGeometry {
    Ogre::RenderOperation renderOp;
    RocketOgreTexture* texture;
};

RocketInterface::RocketInterface(Renderer* rs, Ogre::MaterialPtr uiMaterial,
                                 const Ogre::Matrix4& projection)
    : mRenderSystem(rs->getOgreRenderSystem()),
      mSceneMgr(rs->getSceneMgr()),
      mUIMaterial(uiMaterial),
      mProjection(projection) {
    mScissorEnable = false;

    mScissorLeft = 0;
    mScissorTop = 0;
    mScissorRight = (int)rs->getWidth();
    mScissorBottom = (int)rs->getHeight();

    setupKeymap();
}

RocketInterface::~RocketInterface() {
}

int RocketInterface::mapSDLKeyCode(SDL_Keycode key) {
    auto it = mKeyMap.find(key);
    if (it != mKeyMap.end())
        return it->second;
    else
        return Rocket::Core::Input::KI_UNKNOWN;
}

Rocket::Core::Input::KeyModifier RocketInterface::mapSDLKeyMod(Uint16 mod) {
    using namespace Rocket::Core::Input;

    int rocketMod = 0;
    if (mod & KMOD_SHIFT)
        rocketMod |= KM_SHIFT;
    if (mod & KMOD_CTRL)
        rocketMod |= KM_CTRL;
    if (mod & KMOD_ALT)
        rocketMod |= KM_ALT;
    if (mod & KMOD_GUI)
        rocketMod |= KM_META;
    if (mod & KMOD_CAPS)
        rocketMod |= KM_CAPSLOCK;
    if (mod & KMOD_NUM)
        rocketMod |= KM_NUMLOCK;

    return KeyModifier(rocketMod);
}

int RocketInterface::mapSDLMouseButton(uint button) {
    return button - 1;
}

void RocketInterface::RenderGeometry(Rocket::Core::Vertex* vertices, int numVertices, int* indices,
                                     int numIndices, Rocket::Core::TextureHandle texture,
                                     const Rocket::Core::Vector2f& translation) {
    // NOTE: This is very inefficient so it should only be used when debugging
    Rocket::Core::CompiledGeometryHandle gh =
        CompileGeometry(vertices, numVertices, indices, numIndices, texture);
    RenderCompiledGeometry(gh, translation);
    ReleaseCompiledGeometry(gh);
}

Rocket::Core::CompiledGeometryHandle RocketInterface::CompileGeometry(
    Rocket::Core::Vertex* vertices, int numVertices, int* indices, int numIndices,
    Rocket::Core::TextureHandle texture) {
    RocketOgreGeometry* geometry = new RocketOgreGeometry();
    geometry->texture = texture == 0 ? nullptr : reinterpret_cast<RocketOgreTexture*>(texture);
    geometry->renderOp.vertexData = new Ogre::VertexData();
    geometry->renderOp.vertexData->vertexStart = 0;
    geometry->renderOp.vertexData->vertexCount = numVertices;
    geometry->renderOp.indexData = new Ogre::IndexData();
    geometry->renderOp.indexData->indexStart = 0;
    geometry->renderOp.indexData->indexCount = numIndices;
    geometry->renderOp.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;

    // Set up the vertex declaration
    Ogre::VertexDeclaration* vertexDecl = geometry->renderOp.vertexData->vertexDeclaration;
    size_t offset = 0;
    vertexDecl->addElement(0, offset, Ogre::VET_FLOAT2, Ogre::VES_POSITION);
    offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT2);
    vertexDecl->addElement(0, offset, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);
    offset += Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR);
    vertexDecl->addElement(0, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);

    // Create the vertex buffer
    Ogre::HardwareVertexBufferSharedPtr vb =
        Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
            vertexDecl->getVertexSize(0), numVertices, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
    geometry->renderOp.vertexData->vertexBufferBinding->setBinding(0, vb);

    // Fill the vertex buffer
    RocketOgreVertex* vertexData = static_cast<RocketOgreVertex*>(
        vb->lock(0, vb->getSizeInBytes(), Ogre::HardwareBuffer::HBL_NORMAL));
    for (int i = 0; i < numVertices; ++i) {
        vertexData[i].position.x = vertices[i].position.x;
        vertexData[i].position.y = vertices[i].position.y;

        vertexData[i].uv.x = vertices[i].tex_coord[0];
        vertexData[i].uv.y = vertices[i].tex_coord[1];

        // Calculate colour value
        Ogre::ColourValue diffuse(
            vertices[i].colour.red / 255.0f, vertices[i].colour.green / 255.0f,
            vertices[i].colour.blue / 255.0f, vertices[i].colour.alpha / 255.0f);

        // Scale colour by gamma value (2.2)
        diffuse.r = pow(diffuse.r, 2.2f);
        diffuse.g = pow(diffuse.g, 2.2f);
        diffuse.b = pow(diffuse.b, 2.2f);
        diffuse.a = pow(diffuse.a, 2.2f);

        mRenderSystem->convertColourValue(diffuse, &vertexData[i].colour);
    }
    vb->unlock();

    // Create the index buffer
    Ogre::HardwareIndexBufferSharedPtr ib =
        Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
            Ogre::HardwareIndexBuffer::IT_32BIT, numIndices,
            Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
    geometry->renderOp.indexData->indexBuffer = ib;
    geometry->renderOp.useIndexes = true;

    // Fill the index buffer
    void* indexData = ib->lock(0, ib->getSizeInBytes(), Ogre::HardwareBuffer::HBL_NORMAL);
    memcpy(indexData, indices, sizeof(unsigned int) * numIndices);
    ib->unlock();

    return reinterpret_cast<Rocket::Core::CompiledGeometryHandle>(geometry);
}

void RocketInterface::RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometryHandle,
                                             const Rocket::Core::Vector2f& translation) {
    RocketOgreGeometry* geometry = reinterpret_cast<RocketOgreGeometry*>(geometryHandle);

    // Build world matrix
    Ogre::Matrix4 world;
    world.makeTrans(translation.x, translation.y, 0);

    // Draw UI element
    Ogre::Pass* pass;
    if (geometry->texture) {
        pass = mUIMaterial->getTechnique("Texture")->getPass(0);
        pass->getTextureUnitState(0)->setTexture(geometry->texture->texture);
    } else {
        pass = mUIMaterial->getTechnique("NoTexture")->getPass(0);
    }
    mSceneMgr->manualRender(&geometry->renderOp, pass, nullptr, world, Ogre::Matrix4::IDENTITY,
                            mProjection);
}

void RocketInterface::ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometryHandle) {
    RocketOgreGeometry* geometry = reinterpret_cast<RocketOgreGeometry*>(geometryHandle);
    delete geometry->renderOp.vertexData;
    delete geometry->renderOp.indexData;
    delete geometry;
}

void RocketInterface::EnableScissorRegion(bool enable) {
    mScissorEnable = enable;

    if (!mScissorEnable)
        mRenderSystem->setScissorTest(false);
    else
        mRenderSystem->setScissorTest(true, mScissorLeft, mScissorTop, mScissorRight,
                                      mScissorBottom);
}

void RocketInterface::SetScissorRegion(int x, int y, int width, int height) {
    mScissorLeft = x;
    mScissorTop = y;
    mScissorRight = x + width;
    mScissorBottom = y + height;

    if (mScissorEnable)
        mRenderSystem->setScissorTest(true, mScissorLeft, mScissorTop, mScissorRight,
                                      mScissorBottom);
}

bool RocketInterface::LoadTexture(Rocket::Core::TextureHandle& textureHandle,
                                  Rocket::Core::Vector2i& textureDimensions,
                                  const Rocket::Core::String& source) {
    Ogre::TextureManager* tm = Ogre::TextureManager::getSingletonPtr();
    Ogre::TexturePtr texture = tm->getByName(Ogre::String(source.CString()));

    if (texture.isNull()) {
        texture = tm->load(Ogre::String(source.CString()),
                           Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                           Ogre::TEX_TYPE_2D, 0, 1.0f, false, Ogre::PF_UNKNOWN, true);
    }

    if (texture.isNull())
        return false;

    textureDimensions.x = texture->getWidth();
    textureDimensions.y = texture->getHeight();

    textureHandle = reinterpret_cast<Rocket::Core::TextureHandle>(new RocketOgreTexture(texture));
    return true;
}

bool RocketInterface::GenerateTexture(Rocket::Core::TextureHandle& textureHandle,
                                      const Rocket::Core::byte* source,
                                      const Rocket::Core::Vector2i& dimensions) {
    static int id = 1;

    Ogre::DataStreamPtr stream(OGRE_NEW Ogre::MemoryDataStream(
        (void*)source, dimensions.x * dimensions.y * sizeof(unsigned int)));
    Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().loadRawData(
        Rocket::Core::String(16, "%d", id++).CString(),
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, stream, (Ogre::ushort)dimensions.x,
        (Ogre::ushort)dimensions.y, Ogre::PF_A8B8G8R8, Ogre::TEX_TYPE_2D, 0);

    if (texture.isNull())
        return false;

    textureHandle = reinterpret_cast<Rocket::Core::TextureHandle>(new RocketOgreTexture(texture));
    return true;
}

void RocketInterface::ReleaseTexture(Rocket::Core::TextureHandle texture) {
    delete reinterpret_cast<RocketOgreTexture*>(texture);
}

float RocketInterface::GetHorizontalTexelOffset() {
    return -mRenderSystem->getHorizontalTexelOffset();
}

float RocketInterface::GetVerticalTexelOffset() {
    return -mRenderSystem->getVerticalTexelOffset();
}

Rocket::Core::FileHandle RocketInterface::Open(const Rocket::Core::String& path) {
    Ogre::DataStreamPtr stream =
        Ogre::ResourceGroupManager::getSingleton().openResource(path.CString());

    if (stream.isNull())
        return 0;

    return reinterpret_cast<Rocket::Core::FileHandle>(new Ogre::DataStreamPtr(stream));
}

void RocketInterface::Close(Rocket::Core::FileHandle file) {
    if (!file)
        return;

    Ogre::DataStreamPtr* pstream = reinterpret_cast<Ogre::DataStreamPtr*>(file);
    delete pstream;
}

size_t RocketInterface::Read(void* buffer, size_t size, Rocket::Core::FileHandle file) {
    if (!file)
        return 0;

    Ogre::DataStreamPtr stream = *reinterpret_cast<Ogre::DataStreamPtr*>(file);
    return stream->read(buffer, size);
}

bool RocketInterface::Seek(Rocket::Core::FileHandle file, long offset, int origin) {
    if (!file)
        return false;

    Ogre::DataStreamPtr stream = *reinterpret_cast<Ogre::DataStreamPtr*>(file);
    long pos = 0;
    size_t size = stream->size();

    if (origin == SEEK_CUR)
        pos = stream->tell() + offset;
    else if (origin == SEEK_END)
        pos = size + offset;
    else
        pos = offset;

    if (pos < 0 || pos > (long)size)
        return false;

    stream->seek((size_t)pos);
    return true;
}

size_t RocketInterface::Tell(Rocket::Core::FileHandle file) {
    if (!file)
        return 0;

    Ogre::DataStreamPtr stream = *reinterpret_cast<Ogre::DataStreamPtr*>(file);
    return stream->tell();
}

float RocketInterface::GetElapsedTime() {
    return static_cast<float>(time::now());
}

bool RocketInterface::LogMessage(Rocket::Core::Log::Type type,
                                 const Rocket::Core::String& message) {
    assert(Log::ptr());
    LOG << "[librocket]: " << message.CString();
    return false;
}

void RocketInterface::setupKeymap() {
    // Set up key mappings
    using namespace Rocket::Core::Input;
    mKeyMap[SDLK_UNKNOWN] = KI_UNKNOWN;
    mKeyMap[SDLK_SPACE] = KI_SPACE;
    mKeyMap[SDLK_0] = KI_0;
    mKeyMap[SDLK_1] = KI_1;
    mKeyMap[SDLK_2] = KI_2;
    mKeyMap[SDLK_3] = KI_3;
    mKeyMap[SDLK_4] = KI_4;
    mKeyMap[SDLK_5] = KI_5;
    mKeyMap[SDLK_6] = KI_6;
    mKeyMap[SDLK_7] = KI_7;
    mKeyMap[SDLK_8] = KI_8;
    mKeyMap[SDLK_9] = KI_9;
    mKeyMap[SDLK_a] = KI_A;
    mKeyMap[SDLK_b] = KI_B;
    mKeyMap[SDLK_c] = KI_C;
    mKeyMap[SDLK_d] = KI_D;
    mKeyMap[SDLK_e] = KI_E;
    mKeyMap[SDLK_f] = KI_F;
    mKeyMap[SDLK_g] = KI_G;
    mKeyMap[SDLK_h] = KI_H;
    mKeyMap[SDLK_i] = KI_I;
    mKeyMap[SDLK_j] = KI_J;
    mKeyMap[SDLK_k] = KI_K;
    mKeyMap[SDLK_l] = KI_L;
    mKeyMap[SDLK_m] = KI_M;
    mKeyMap[SDLK_n] = KI_N;
    mKeyMap[SDLK_o] = KI_O;
    mKeyMap[SDLK_p] = KI_P;
    mKeyMap[SDLK_q] = KI_Q;
    mKeyMap[SDLK_r] = KI_R;
    mKeyMap[SDLK_s] = KI_S;
    mKeyMap[SDLK_t] = KI_T;
    mKeyMap[SDLK_u] = KI_U;
    mKeyMap[SDLK_v] = KI_V;
    mKeyMap[SDLK_w] = KI_W;
    mKeyMap[SDLK_x] = KI_X;
    mKeyMap[SDLK_y] = KI_Y;
    mKeyMap[SDLK_z] = KI_Z;
    mKeyMap[SDLK_SEMICOLON] = KI_OEM_1;
    mKeyMap[SDLK_PLUS] = KI_OEM_PLUS;
    mKeyMap[SDLK_COMMA] = KI_OEM_COMMA;
    mKeyMap[SDLK_MINUS] = KI_OEM_MINUS;
    mKeyMap[SDLK_PERIOD] = KI_OEM_PERIOD;
    mKeyMap[SDLK_SLASH] = KI_OEM_2;
    mKeyMap[SDLK_BACKQUOTE] = KI_OEM_3;
    mKeyMap[SDLK_LEFTBRACKET] = KI_OEM_4;
    mKeyMap[SDLK_BACKSLASH] = KI_OEM_5;
    mKeyMap[SDLK_RIGHTBRACKET] = KI_OEM_6;
    mKeyMap[SDLK_QUOTEDBL] = KI_OEM_7;
    mKeyMap[SDLK_KP_0] = KI_NUMPAD0;
    mKeyMap[SDLK_KP_1] = KI_NUMPAD1;
    mKeyMap[SDLK_KP_2] = KI_NUMPAD2;
    mKeyMap[SDLK_KP_3] = KI_NUMPAD3;
    mKeyMap[SDLK_KP_4] = KI_NUMPAD4;
    mKeyMap[SDLK_KP_5] = KI_NUMPAD5;
    mKeyMap[SDLK_KP_6] = KI_NUMPAD6;
    mKeyMap[SDLK_KP_7] = KI_NUMPAD7;
    mKeyMap[SDLK_KP_8] = KI_NUMPAD8;
    mKeyMap[SDLK_KP_9] = KI_NUMPAD9;
    mKeyMap[SDLK_KP_ENTER] = KI_NUMPADENTER;
    mKeyMap[SDLK_KP_MULTIPLY] = KI_MULTIPLY;
    mKeyMap[SDLK_KP_PLUS] = KI_ADD;
    mKeyMap[SDLK_KP_MINUS] = KI_SUBTRACT;
    mKeyMap[SDLK_KP_PERIOD] = KI_DECIMAL;
    mKeyMap[SDLK_KP_DIVIDE] = KI_DIVIDE;
    mKeyMap[SDLK_KP_EQUALS] = KI_OEM_NEC_EQUAL;
    mKeyMap[SDLK_BACKSPACE] = KI_BACK;
    mKeyMap[SDLK_TAB] = KI_TAB;
    mKeyMap[SDLK_CLEAR] = KI_CLEAR;
    mKeyMap[SDLK_RETURN] = KI_RETURN;
    mKeyMap[SDLK_PAUSE] = KI_PAUSE;
    mKeyMap[SDLK_CAPSLOCK] = KI_CAPITAL;
    mKeyMap[SDLK_PAGEUP] = KI_PRIOR;
    mKeyMap[SDLK_PAGEDOWN] = KI_NEXT;
    mKeyMap[SDLK_END] = KI_END;
    mKeyMap[SDLK_HOME] = KI_HOME;
    mKeyMap[SDLK_LEFT] = KI_LEFT;
    mKeyMap[SDLK_UP] = KI_UP;
    mKeyMap[SDLK_RIGHT] = KI_RIGHT;
    mKeyMap[SDLK_DOWN] = KI_DOWN;
    mKeyMap[SDLK_INSERT] = KI_INSERT;
    mKeyMap[SDLK_DELETE] = KI_DELETE;
    mKeyMap[SDLK_HELP] = KI_HELP;
    mKeyMap[SDLK_F1] = KI_F1;
    mKeyMap[SDLK_F2] = KI_F2;
    mKeyMap[SDLK_F3] = KI_F3;
    mKeyMap[SDLK_F4] = KI_F4;
    mKeyMap[SDLK_F5] = KI_F5;
    mKeyMap[SDLK_F6] = KI_F6;
    mKeyMap[SDLK_F7] = KI_F7;
    mKeyMap[SDLK_F8] = KI_F8;
    mKeyMap[SDLK_F9] = KI_F9;
    mKeyMap[SDLK_F10] = KI_F10;
    mKeyMap[SDLK_F11] = KI_F11;
    mKeyMap[SDLK_F12] = KI_F12;
    mKeyMap[SDLK_F13] = KI_F13;
    mKeyMap[SDLK_F14] = KI_F14;
    mKeyMap[SDLK_F15] = KI_F15;
    mKeyMap[SDLK_NUMLOCKCLEAR] = KI_NUMLOCK;
    mKeyMap[SDLK_SCROLLLOCK] = KI_SCROLL;
    mKeyMap[SDLK_LSHIFT] = KI_LSHIFT;
    mKeyMap[SDLK_RSHIFT] = KI_RSHIFT;
    mKeyMap[SDLK_LCTRL] = KI_LCONTROL;
    mKeyMap[SDLK_RCTRL] = KI_RCONTROL;
    mKeyMap[SDLK_LALT] = KI_LMENU;
    mKeyMap[SDLK_RALT] = KI_RMENU;
    mKeyMap[SDLK_LGUI] = KI_LMETA;
    mKeyMap[SDLK_RGUI] = KI_RMETA;
}
}  // namespace dw
