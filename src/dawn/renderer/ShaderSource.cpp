/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "io/InputStream.h"
#include "renderer/ShaderSource.h"

namespace dw {

ShaderSource::ShaderSource(Context* context) : Resource(context) {
}

ShaderSource::~ShaderSource() {
}

bool ShaderSource::beginLoad(InputStream& src) {
    // TODO(David): Sanity checking here?
    mSrcLen = (u32)src.getSize();
    assert(mSrcLen != 0);
    mSrcData = new byte[mSrcLen];
    src.read(mSrcData, src.getSize());

    // Does this require compilation?
    /// TODO(David): Execute shaderc on source file

    return true;
}

void ShaderSource::endLoad() {
    // Create bgfx shader and free memory
    mHandle = bgfx::createShader(bgfx::makeRef(mSrcData, mSrcLen));
    delete[] mSrcData;
    mSrcData = nullptr;
}

bgfx::ShaderHandle ShaderSource::getHandle() const {
    return mHandle;
}
}
