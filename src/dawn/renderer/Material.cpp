/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Material.h"

namespace dw {

Material::Material(Context* context, SharedPtr<Program> program)
    : Object{context}, program_{program}, polygon_mode_{PolygonMode::Fill} {
}

Material::~Material() {
}

void Material::setPolygonMode(PolygonMode polygon_mode)
{
    polygon_mode_ = polygon_mode;
}

void Material::setTexture(SharedPtr<Texture> texture, uint unit) {
    program_->setTextureUnit(texture, unit);
}

void Material::applyRendererState(const Mat4& model_matrix, const Mat4& view_projection_matrix)
{
    Renderer* renderer = subsystem<Renderer>();

    // Bind render state.
    renderer->setStatePolygonMode(polygon_mode_);

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
}  // namespace dw
