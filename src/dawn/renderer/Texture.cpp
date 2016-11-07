/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "renderer/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace dw {

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

Texture::Texture(Context* context) {
}

Texture::~Texture() {
}

bool Texture::beginLoad(InputStream& src) override {
    stbi_io_callbacks callbacks;
    callbacks.read = &imageCallbackRead;
    callbacks.skip = &imageCallbackSkip;
    callbacks.eof = &imageCallbackEof;

    int width, height, bpp;
    byte* data = stbi_load_from_callbacks(&callbacks, reinterpret_cast<void*>(&src), &x, &y, bpp, 4);
}

void Texture::endLoad() override {

}

}
