/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "core/io/InputStream.h"
#include "renderer/Shader.h"
#include "renderer/Renderer.h"

#include <dawn-gfx/Shader.h>

namespace dw {
Shader::Shader(Context* context, gfx::ShaderStage type, const Vector<String>& compile_definitions)
    : Resource{context}, type_{type}, compile_definitions_(compile_definitions) {
}

Result<void> Shader::beginLoad(const String&, InputStream& src) {
    u32 src_len = static_cast<u32>(src.size());
    assert(src_len != 0);
    std::string src_data;
    src_data.resize(src_len);
    src.readData(src_data.data(), src_len);

    auto result = gfx::compileGLSL(src_data, type_, compile_definitions_);
    if (!result) {
        return makeError("Failed to compile shader: " + result.error().compile_error);
    }
    handle_ = module<Renderer>()->gfx()->createShader(type_, result.value().entry_point,
                                                      gfx::Memory(result.value().spirv));
    return {};
}

gfx::ShaderHandle Shader::internalHandle() const {
    return handle_;
}
}  // namespace dw
