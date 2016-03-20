/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include <Rocket/Core/FileInterface.h>
#include <Rocket/Core/RenderInterface.h>
#include <Rocket/Core/SystemInterface.h>

NAMESPACE_BEGIN

class Renderer;

struct RocketOgreVertex
{
    Ogre::Vector2 position;
    Ogre::uint32 colour;
    Ogre::Vector2 uv;
};

class RocketInterface : public Rocket::Core::RenderInterface,
                        public Rocket::Core::FileInterface,
                        public Rocket::Core::SystemInterface
{
public:
    RocketInterface(Renderer* rs, Ogre::MaterialPtr uiMaterial, const Ogre::Matrix4& projection);
    virtual ~RocketInterface();

    /// Maps an SDL keycode to libRocket
    /// @param  key SDL keycode
    /// @return The equivalent libRocket keycode
    int mapSDLKeyCode(SDL_Keycode key);

    /// Maps an SDL key modifier to libRocket
    /// @param  mod SDL modifier
    /// @return The equivalent libRocket modifier
    Rocket::Core::Input::KeyModifier mapSDLKeyMod(Uint16 mod);

    /// Maps an SDL mouse button to libRocket
    /// @param  button SDL mouse button identifier
    /// @return The equivalent libRocket mouse button identifier
    int mapSDLMouseButton(uint button);

    // Inherited from Rocket::Core::RenderInterface
    void Transform(RocketOgreVertex* vertex);
    virtual void RenderGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices,
                                int num_indices, Rocket::Core::TextureHandle texture,
                                const Rocket::Core::Vector2f& translation) override;
    virtual Rocket::Core::CompiledGeometryHandle CompileGeometry(
        Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices,
        Rocket::Core::TextureHandle texture) override;
    virtual void RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry,
                                        const Rocket::Core::Vector2f& translation) override;
    virtual void ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry) override;
    virtual void EnableScissorRegion(bool enable) override;
    virtual void SetScissorRegion(int x, int y, int width, int height) override;
    virtual bool LoadTexture(Rocket::Core::TextureHandle& texture_handle,
                             Rocket::Core::Vector2i& texture_dimensions,
                             const Rocket::Core::String& source) override;
    virtual bool GenerateTexture(Rocket::Core::TextureHandle& texture_handle,
                                 const Rocket::Core::byte* source,
                                 const Rocket::Core::Vector2i& source_dimensions) override;
    virtual void ReleaseTexture(Rocket::Core::TextureHandle texture) override;
    float GetHorizontalTexelOffset() override;
    float GetVerticalTexelOffset() override;

    // Inherited from Rocket::Core::FileInterface
    virtual Rocket::Core::FileHandle Open(const Rocket::Core::String& path) override;
    virtual void Close(Rocket::Core::FileHandle file) override;
    virtual size_t Read(void* buffer, size_t size, Rocket::Core::FileHandle file) override;
    virtual bool Seek(Rocket::Core::FileHandle file, long offset, int origin) override;
    virtual size_t Tell(Rocket::Core::FileHandle file) override;

    // Inherited from Rocket::Core::SystemInterface
    virtual float GetElapsedTime() override;
    virtual bool LogMessage(Rocket::Core::Log::Type type, const Rocket::Core::String& message) override;

private:
    Ogre::RenderSystem* mRenderSystem;
    Ogre::SceneManager* mSceneMgr;
    Ogre::MaterialPtr mUIMaterial;
    Ogre::Matrix4 mProjection;

    Map<SDL_Keycode, int> mKeyMap;

    bool mScissorEnable;
    int mScissorLeft;
    int mScissorTop;
    int mScissorRight;
    int mScissorBottom;

private:
    void setupKeymap();

};

NAMESPACE_END
