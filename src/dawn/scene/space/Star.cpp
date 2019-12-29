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
    : SystemBody(ctx, desc.radius, system_node),
      desc_(desc),
      flare_system_node_(*system_node.newChild()) {
    auto& rc = *module<ResourceCache>();

    auto surface_vs = rc.getUnchecked<VertexShader>("base:space/star_surface.vs");
    auto surface_fs = rc.getUnchecked<FragmentShader>("base:space/star_surface.fs");

    // Set up surface material.
    surface_material_ =
        makeShared<Material>(context(), makeShared<Program>(context(), surface_vs, surface_fs));

    // Create surface.
    auto surface_renderable =
        CustomRenderable::Builder(context()).texcoords(true).createSphere(desc.radius, 24, 24);
    surface_renderable->setMaterial(surface_material_);
    system_node.data.renderable = surface_renderable;

    // Load star flare material.
    flare_material_ = makeShared<Material>(
        context(),
        makeShared<Program>(
            context(), rc.getUnchecked<VertexShader>("base:materials/post_process.vs"),
            rc.getUnchecked<FragmentShader>("base:materials/post_process_star_flare.fs")));
    flare_material_->enableAdditiveBlending();
    flare_material_->setUniform("aspect_ratio", 1280.0f / 800.0f);
    flare_material_->setDepthWrite(false);
    flare_material_->setStateDisable(gfx::RenderState::Depth);

    // Create fullscreen quad mesh.
    // clang-format off
    float vertices[] = {
        // Position | UV
        -1.0f, -1.0f, 0.0f, 0.0f,
        3.0f,  -1.0f, 2.0f, 0.0f,
        -1.0f,  3.0f, 0.0f, 2.0f
    };
    // clang-format on
    gfx::VertexDecl decl;
    decl.begin()
        .add(gfx::VertexDecl::Attribute::Position, 2, gfx::VertexDecl::AttributeType::Float)
        .add(gfx::VertexDecl::Attribute::TexCoord0, 2, gfx::VertexDecl::AttributeType::Float)
        .end();

    auto flare_renderable = makeShared<CustomRenderable>(
        ctx, makeShared<VertexBuffer>(ctx, gfx::Memory(vertices, sizeof(vertices)), 3, decl));
    flare_renderable->setMaterial(flare_material_);
    flare_renderable->setRenderQueueGroup(RenderQueueGroup::Group7);
    flare_system_node_.data.renderable = flare_renderable;
}

void Star::preRender(Frame& frame, const Mat4& view_matrix, const Mat4& proj_matrix) {
    SystemBody::preRender(frame, view_matrix, proj_matrix);

    SystemPosition star_position = system_node_.position;
    Vec3 frame_local_star_position = star_position.getRelativeTo(frame.position());

    // Project star position and update visibility flag.
    bool visible = false;
    Vec3 projected_star_position =
        projectPoint(frame_local_star_position, proj_matrix * view_matrix, &visible);
    flare_system_node_.data.renderable->setVisible(visible);
    if (!visible) {
        return;
    }

    // Project to UV coordinates.
    Vec2 star_uv_coords =
        convertToViewport(projected_star_position, Vec2{0.0f, 0.0f}, Vec2{1.0f, 1.0f}).xy();
    flare_material_->setUniform("star_location", star_uv_coords);

    // Update radius.
    float distance = view_matrix.Transform(Vec4{frame_local_star_position, 1.0f}).Length3();
    float star_radius = computeProjectedRadius(proj_matrix, distance, radius_);
    flare_material_->setUniform("star_radius", star_radius * 0.5f);
}
}  // namespace dw
