/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "io/InputStream.h"
#include "renderer/ShaderProgram.h"

namespace dw {
ShaderProgram::ShaderProgram(Context* context) : Resource(context) {
}

ShaderProgram::~ShaderProgram() {
}

bool ShaderProgram::beginLoad(InputStream& src) {
    // TODO(David): Sanity checking here?
    mSrcLen = (u32)src.getSize();
    assert(mSrcLen != 0);
    mSrcData = new byte[mSrcLen];
    src.read(mSrcData, src.getSize());

    // Does this require compilation?
    /// TODO(David): Execute shaderc on source file

    return true;
}

void ShaderProgram::endLoad() {
    // Create bgfx shader and free memory
    mHandle = bgfx::createShader(bgfx::makeRef(mSrcData, mSrcLen));
    delete[] mSrcData;
    mSrcData = nullptr;
}

bgfx::ShaderHandle ShaderProgram::getHandle() const {
    return mHandle;
}
}
