/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "scene/space/Star.h"
#include "renderer/Renderer.h"
#include "renderer/Shader.h"
#include "renderer/Material.h"
#include "renderer/CustomRenderable.h"
#include "resource/ResourceCache.h"

namespace dw {
Star::Star(Context* ctx, SystemNode& system_node, const StarDesc& desc)
    : SystemBody(ctx, desc.radius, system_node), desc_(desc) {
    auto* rc = module<ResourceCache>();

    auto surface_vs = *rc->get<VertexShader>("base:space/star_surface.vs");
    auto surface_fs = *rc->get<FragmentShader>("base:space/star_surface.fs");

    // Set up surface material.
    surface_material_ =
        makeShared<Material>(context(), makeShared<Program>(context(), surface_vs, surface_fs));

    // Create surface.
    auto surface_renderable =
        CustomRenderable::Builder(context()).texcoords(true).createSphere(desc.radius, 24, 24);
    surface_renderable->setMaterial(surface_material_);
    system_node.data.renderable = surface_renderable;
}

Star::~Star() {
}

void Star::preRender() {
}
}  // namespace dw
