/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "io/InputStream.h"
#include "renderer/ShaderProgram.h"

namespace dw {
ShaderProgram::ShaderProgram(Context* context)
    : Resource{context}, src_data_{nullptr}, src_len_{0} {
}

ShaderProgram::~ShaderProgram() {
}

bool ShaderProgram::beginLoad(InputStream& src) {
    // TODO(David): Sanity checking here?
    src_len_ = static_cast<u32>(src.getSize());
    assert(src_len_ != 0);
    src_data_ = new byte[src_len_];
    src.read(src_data_, src_len_);

    // Does this require compilation?
    /// TODO(David): Execute shaderc on source file

    return true;
}

void ShaderProgram::endLoad() {
    // Create bgfx shader and free memory
    handle_ = bgfx::createShader(bgfx::makeRef(src_data_, src_len_));
    delete[] src_data_;
    src_data_ = nullptr;
}

bgfx::ShaderHandle ShaderProgram::internalHandle() const {
    return handle_;
}
}  // namespace dw
