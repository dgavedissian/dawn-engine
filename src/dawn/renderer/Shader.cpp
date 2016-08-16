#include "Common.h"
#include "io/InputStream.h"
#include "resource/Resource.h"
#include "renderer/Shader.h"

namespace dw {

Shader::Shader(Context* context) : Resource(context) {
}

Shader::~Shader() {
}

bool Shader::beginLoad(InputStream& src) {
    // TODO(David): Sanity checking here?
    mSrcLen = (u32)src.getSize();
    assert(mSrcLen != 0);
    mSrcData = new byte[mSrcLen];
    src.read(mSrcData, src.getSize());

    // Does this require compilation?
    /// TODO(David): Execute shaderc on source file

    return true;
}

void Shader::endLoad() {
    // Create bgfx shader and free memory
    mHandle = bgfx::createShader(bgfx::makeRef(mSrcData, mSrcLen));
    delete[] mSrcData;
    mSrcData = nullptr;
}

bgfx::ShaderHandle Shader::getHandle() const {
    return mHandle;
}
}
