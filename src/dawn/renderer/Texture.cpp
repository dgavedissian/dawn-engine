/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "renderer/StbImage.h"
#include "renderer/Texture.h"
#include "renderer/Renderer.h"

namespace dw {

// Internal.
namespace {
int imageCallbackRead(void* user, char* data, int size) {
    InputStream& stream = *reinterpret_cast<InputStream*>(user);
    // If we want to read past the end of the buffer, clamp the size to prevent an error occurring,
    // as the stream API will read either the entire block or nothing at all.
    if ((stream.position() + size) > stream.size()) {
        size = static_cast<int>(stream.size() - stream.position());
    }
    return static_cast<int>(stream.readData(data, static_cast<u32>(size)));
}

void imageCallbackSkip(void* user, int n) {
    InputStream& stream = *reinterpret_cast<InputStream*>(user);
    stream.seek(stream.position() + n);
}

int imageCallbackEof(void* user) {
    InputStream& stream = *reinterpret_cast<InputStream*>(user);
    return stream.eof() ? 1 : 0;
}
}  // namespace

Texture::Texture(Context* ctx) : Resource(ctx) {
}

Texture::~Texture() {
    if (handle_.isValid()) {
        module<Renderer>()->rhi()->deleteTexture(handle_);
    }
}

SharedPtr<Texture> Texture::createTexture2D(Context* ctx, const Vec2i& size,
                                            gfx::TextureFormat format, gfx::Memory data) {
    auto texture = makeShared<Texture>(ctx);
    texture->handle_ =
        ctx->module<Renderer>()->rhi()->createTexture2D(size.x, size.y, format, std::move(data));
    return texture;
}

Result<void> Texture::beginLoad(const String&, InputStream& src) {
    stbi_io_callbacks callbacks = {
        &imageCallbackRead,
        &imageCallbackSkip,
        &imageCallbackEof,
    };
    int width, height, bpp;
    byte* buffer = stbi_load_from_callbacks(&callbacks, reinterpret_cast<void*>(&src), &width,
                                            &height, &bpp, 4);
    gfx::Memory data(buffer, static_cast<usize>(width * height * 4),
                     [](std::byte* buffer) { stbi_image_free(buffer); });
    handle_ = module<Renderer>()->rhi()->createTexture2D(
        static_cast<u16>(width), static_cast<u16>(height), gfx::TextureFormat::RGBA8,
        std::move(data));
    return Result<void>();
}

gfx::TextureHandle Texture::internalHandle() const {
    return handle_;
}
}  // namespace dw
