/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Program.h"
#include "renderer/Renderer.h"

namespace dw {

Program::Program(Context* ctx, SharedPtr<VertexShader> vs, SharedPtr<FragmentShader> fs)
    : Resource{ctx}, r{module<Renderer>()->rhi()}, vertex_shader_{vs}, fragment_shader_{fs} {
    handle_ = r->createProgram();
    r->attachShader(handle_, vs->internalHandle());
    r->attachShader(handle_, fs->internalHandle());
    r->linkProgram(handle_);
}

Program::~Program() {
}

Result<None> Program::beginLoad(const String&, InputStream&) {
    return {"Program loading unimplemented."};
}

void Program::setTextureUnit(SharedPtr<Texture> texture, uint unit) {
    texture_units_[unit] = std::move(texture);
}

rhi::ProgramHandle Program::internalHandle() const {
    return handle_;
}

void Program::applyRendererState() {
    // Set textures.
    for (uint i = 0; i < static_cast<uint>(texture_units_.size()); i++) {
        if (!texture_units_[i]) {
            break;
        }
        r->setTexture(texture_units_[i]->internalHandle(), i);
    }

    // Set uniforms.
    for (auto& uniform_pair : uniforms_) {
        r->setUniform(uniform_pair.first, uniform_pair.second);
    }
    uniforms_.clear();
}
}  // namespace dw
