/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Material.h"
#include "renderer/Renderer.h"

namespace dw {

Material::Material(Context* ctx) : Material{ctx, nullptr} {
}

Material::Material(Context* ctx, SharedPtr<Program> program)
    : Resource{ctx},
      program_{program},
      polygon_mode_{rhi::PolygonMode::Fill},
      depth_write_{true},
      mask_{0x0} {
}

Material::~Material() {
}

Result<None> Material::beginLoad(const String& asset_name, InputStream& src) {
    return {"Material loading unimplemented"};
}

void Material::setPolygonMode(rhi::PolygonMode polygon_mode) {
    polygon_mode_ = polygon_mode;
}

void Material::setDepthWrite(bool depth_write_enabled) {
    depth_write_ = depth_write_enabled;
}

void Material::setMask(u32 mask) {
    mask_ = mask;
}

void Material::setTexture(SharedPtr<Texture> texture, uint unit) {
    program_->setTextureUnit(texture, unit);
}

void Material::applyRendererState(const Mat4& model_matrix, const Mat4& view_projection_matrix) {
    auto* renderer = module<Renderer>()->rhi();

    // Bind render state.
    renderer->setStatePolygonMode(polygon_mode_);
    renderer->setDepthWrite(depth_write_);

    // Bind common variables.
    // TODO: Maybe bind uniforms by some kind of tag?
    setUniform("model_matrix", model_matrix);
    setUniform("mvp_matrix", view_projection_matrix * model_matrix);

    // Apply program render state (textures and uniforms).
    program_->applyRendererState();
}

Program* Material::program() {
    return program_.get();
}

u32 Material::mask() const {
    return mask_;
}
}  // namespace dw
