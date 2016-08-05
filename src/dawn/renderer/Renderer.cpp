/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "renderer/Renderer.h"

namespace dw {

class BGFXCallback : public bgfx::CallbackI {
public:
    virtual void fatal(bgfx::Fatal::Enum _code, const char* _str) override {
        std::cout << _str << std::endl;
    }

    virtual void traceVargs(const char* _filePath, uint16_t _line, const char* _format,
                            va_list _argList) override {
    }

    virtual uint32_t cacheReadSize(uint64_t _id) override {
        return 0;
    }

    virtual bool cacheRead(uint64_t _id, void* _data, uint32_t _size) override {
        return 0;
    }

    virtual void cacheWrite(uint64_t _id, const void* _data, uint32_t _size) override {
    }

    virtual void screenShot(const char* _filePath, uint32_t _width, uint32_t _height,
                            uint32_t _pitch, const void* _data, uint32_t _size,
                            bool _yflip) override {
    }

    virtual void captureBegin(uint32_t _width, uint32_t _height, uint32_t _pitch,
                              bgfx::TextureFormat::Enum _format, bool _yflip) override {
    }

    virtual void captureEnd() override {
    }

    virtual void captureFrame(const void* _data, uint32_t _size) override {
    }
};

BGFXCallback callback;

Renderer::Renderer(Context* context) : Object(context) {
    bgfx::init(bgfx::RendererType::OpenGL, BGFX_PCI_ID_NONE, 0, &callback);
    bgfx::reset(1280, 720, BGFX_RESET_NONE);

    // Enable debug text.
    bgfx::setDebug(BGFX_DEBUG_TEXT);

    // Set view 0 clear state.
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
}

Renderer::~Renderer() {
}

void Renderer::frame() {
    bgfx::setViewRect(0, 0, 0, 1280, 720);
    bgfx::touch(0);

    // Draw frame
    bgfx::frame();
}
}
