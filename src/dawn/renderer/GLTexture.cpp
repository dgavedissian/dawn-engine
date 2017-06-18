/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/GLTexture.h"

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
    return static_cast<int>(stream.read(data, size));
}

void imageCallbackSkip(void* user, int n) {
    InputStream& stream = *reinterpret_cast<InputStream*>(user);
    stream.seek(stream.getPosition() + n);
}

int imageCallbackEof(void* user) {
    InputStream& stream = *reinterpret_cast<InputStream*>(user);
    return stream.eof() ? 1 : 0;
}
}  // namespace

GLTexture::GLTexture(Context* context) : Resource(context) {
}

GLTexture::~GLTexture() {
}

bool GLTexture::beginLoad(InputStream& src) {
    stbi_io_callbacks callbacks = {
        &imageCallbackRead, &imageCallbackSkip, &imageCallbackEof,
    };
    int width, height, bpp;
    byte* data = stbi_load_from_callbacks(&callbacks, reinterpret_cast<void*>(&src), &width,
                                          &height, &bpp, 4);
    //    handle_ = bgfx::createTexture2D(static_cast<uint16_t>(width),
    //    static_cast<uint16_t>(height), 1,
    //                                    bgfx::TextureFormat::RGBA8, 0,
    //                                    bgfx::copy(data, width * height * bpp));
    stbi_image_free(data);
    return true;
}

void GLTexture::endLoad() {
}

// bgfx::TextureHandle GLTexture::internalHandle() const {
//    return handle_;
//}
}  // namespace dw
