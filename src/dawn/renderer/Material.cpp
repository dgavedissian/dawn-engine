/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "renderer/Material.h"
#include "renderer/Renderer.h"

namespace dw {

Material::Material(Context* ctx) : Material{ctx, nullptr} {
}

// Default render state is taken from RHIRenderer.cpp:40
Material::Material(Context* ctx, SharedPtr<Program> program)
    : Resource{ctx},
      program_{program},
      cull_front_face_{rhi::CullFrontFace::CCW},
      polygon_mode_{rhi::PolygonMode::Fill},
      blend_equation_rgb_{rhi::BlendEquation::Add},
      blend_equation_a_{rhi::BlendEquation::Add},
      blend_src_rgb_{rhi::BlendFunc::One},
      blend_src_a_{rhi::BlendFunc::One},
      blend_dest_rgb_{rhi::BlendFunc::Zero},
      blend_dest_a_{rhi::BlendFunc::Zero},
      colour_write_{true},
      depth_write_{true},
      mask_{0x1} {
}

Material::~Material() {
}

Result<void> Material::beginLoad(const String& asset_name, InputStream& src) {
    return makeError("Material loading unimplemented");
}

void Material::setStateEnable(rhi::RenderState state) {
    states_to_enable_.emplace(state);
    states_to_disable_.erase(state);
}

void Material::setStateDisable(rhi::RenderState state) {
    states_to_disable_.emplace(state);
    states_to_enable_.erase(state);
}

void Material::setCullFrontFace(rhi::CullFrontFace front_face) {
    cull_front_face_ = front_face;
}

void Material::setPolygonMode(rhi::PolygonMode polygon_mode) {
    polygon_mode_ = polygon_mode;
}

void Material::setBlendEquation(rhi::BlendEquation equation, rhi::BlendFunc src,
                                rhi::BlendFunc dest) {
    blend_equation_rgb_ = blend_equation_a_ = equation;
    blend_src_rgb_ = blend_src_a_ = src;
    blend_dest_rgb_ = blend_dest_a_ = dest;
}

void Material::setBlendEquation(rhi::BlendEquation equation_rgb, rhi::BlendFunc src_rgb,
                                rhi::BlendFunc dest_rgb, rhi::BlendEquation equation_a,
                                rhi::BlendFunc src_a, rhi::BlendFunc dest_a) {
    blend_equation_rgb_ = equation_rgb;
    blend_src_rgb_ = src_rgb;
    blend_dest_rgb_ = dest_rgb;
    blend_equation_a_ = equation_a;
    blend_src_a_ = src_a;
    blend_dest_a_ = dest_a;
}

void Material::setColourWrite(bool colour_write_enabled) {
    colour_write_ = colour_write_enabled;
}

void Material::setDepthWrite(bool depth_write_enabled) {
    depth_write_ = depth_write_enabled;
}

void Material::setMask(u32 mask) {
    mask_ = mask;
}

void Material::setTexture(SharedPtr<Texture> texture, uint unit) {
    texture_units_[unit] = std::move(texture);
}

void Material::applyRendererState(const Mat4& model_matrix, const Mat4& view_projection_matrix) {
    auto* renderer = module<Renderer>()->rhi();

    // Bind render state.
    for (auto state : states_to_enable_) {
        renderer->setStateEnable(state);
    }
    for (auto state : states_to_disable_) {
        renderer->setStateDisable(state);
    }
    renderer->setStateCullFrontFace(cull_front_face_);
    renderer->setStatePolygonMode(polygon_mode_);
    renderer->setStateBlendEquation(blend_equation_rgb_, blend_src_rgb_, blend_dest_rgb_,
                                    blend_equation_a_, blend_src_a_, blend_dest_a_);
    renderer->setColourWrite(colour_write_);
    renderer->setDepthWrite(depth_write_);

    // Bind common variables.
    // TODO: Maybe bind uniforms by some kind of tag?
    setUniform("model_matrix", model_matrix);
    setUniform("mvp_matrix", view_projection_matrix * model_matrix);

    // Set textures.
    for (uint i = 0; i < static_cast<uint>(texture_units_.size()); i++) {
        if (!texture_units_[i]) {
            break;
        }
        renderer->setTexture(texture_units_[i]->internalHandle(), i);
    }

    // Set uniforms.
    for (auto& uniform_pair : uniforms_) {
        renderer->setUniform(uniform_pair.first, uniform_pair.second);
    }
    uniforms_.clear();

    // Apply program render state.
    program_->applyRendererState();
}

Program* Material::program() {
    return program_.get();
}

u32 Material::mask() const {
    return mask_;
}
}  // namespace dw
