/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Texture.h"

#if defined(DW_MSVC)
#pragma warning(push)
#pragma warning(disable : 4100 4244)
#endif
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#if defined(DW_MSVC)
#pragma warning(pop)
#endif

namespace dw {

// Internal.
namespace {
int imageCallbackRead(void* user, char* data, int size) {
    InputStream& stream = *reinterpret_cast<InputStream*>(user);
    // If we want to read past the end of the buffer, clamp the size to prevent an error occurring.
    if ((stream.position() + size) > stream.size()) {
        size = static_cast<int>(stream.size() - stream.position());
    }
    return static_cast<int>(stream.read(data, static_cast<u32>(size)));
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

Texture::Texture(Context* context) : Resource(context) {
}

Texture::~Texture() {
}

bool Texture::beginLoad(InputStream& src) {
    stbi_io_callbacks callbacks = {
        &imageCallbackRead, &imageCallbackSkip, &imageCallbackEof,
    };
    int width, height, bpp;
    byte* data = stbi_load_from_callbacks(&callbacks, reinterpret_cast<void*>(&src), &width,
                                          &height, &bpp, 4);
    handle_ = subsystem<Renderer>()->createTexture2D(static_cast<u16>(width),
                                                     static_cast<u16>(height), TextureFormat::RGBA8,
                                                     data, static_cast<u16>(width * height * 4));
    stbi_image_free(data);
    return true;
}

void Texture::endLoad() {
}

TextureHandle Texture::internalHandle() const {
    return handle_;
}
}  // namespace dw
