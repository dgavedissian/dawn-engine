/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "scene/space/PlanetRings.h"
#include "scene/space/Planet.h"
#include "renderer/Renderer.h"
#include "resource/ResourceCache.h"
#include "renderer/CustomRenderable.h"

namespace dw {
PlanetRings::PlanetRings(Context* ctx, const PlanetDesc& desc, SystemNode& system_node)
    : Object(ctx),
      desc_(desc),
      min_radius_(desc.rings.min_radius),
      max_radius_(desc.rings.max_radius),
      ring_system_node_(system_node) {
    // Generate the LOD look up table
    lod_distance_table_[0] = pow(max_radius_ * 2.0f, 2);
    for (usize i = 1; i < lod_distance_table_.size(); i++) {
        lod_distance_table_[i] = lod_distance_table_[i - 1] * 0.25f;
    }

    // Generate a List of points used by the ring mesh
    uint detail = 16;
    Vector<Vec2> vertices;
    float offset = math::Cos(math::pi / static_cast<float>(detail));
    float max_radius = desc.rings.max_radius / offset;
    float min_radius = desc.rings.min_radius * offset;
    for (uint i = 0; i < detail; ++i) {
        float angle = (static_cast<float>(i) / static_cast<float>(detail)) * math::pi * 2.0f;
        Vec2 pos(sin(angle), cos(angle));
        vertices.push_back(pos * max_radius);
        vertices.push_back(pos * min_radius);
    }

    // Create the mesh from the data
    gfx::TriangleBuffer ring_mesh_buffer;
    ring_mesh_buffer.estimateVertexCount(vertices.size());
    ring_mesh_buffer.begin();

    // Add the vertices
    for (const auto& vertex : vertices) {
        ring_mesh_buffer.position({vertex.x, 0.0f, vertex.y});
    }

    // Add the indices
    for (uint i = 0; i < detail; ++i) {
        uint start_index = i * 2;

        if (i < (detail - 1)) {
            ring_mesh_buffer.triangle(start_index, start_index + 2, start_index + 1);
            ring_mesh_buffer.triangle(start_index + 2, start_index + 3, start_index + 1);
        } else {
            ring_mesh_buffer.triangle(start_index, 0, start_index + 1);
            ring_mesh_buffer.triangle(0, 1, start_index + 1);
        }
    }

    // Convert this triangle buffer into a renderable and free the buffers.
    ring_renderable_ =
        makeShared<CustomRenderable>(ctx, ring_mesh_buffer.end(*ctx->module<Renderer>()->gfx()));

    auto* rc = module<ResourceCache>();

    auto ring_vs = rc->getUnchecked<VertexShader>("base:materials/scene/rings.vs");
    auto ring_fs = rc->getUnchecked<FragmentShader>("base:materials/scene/rings.fs");

    // Set up surface material.
    ring_material_ =
        makeShared<Material>(context(), makeShared<Program>(context(), ring_vs, ring_fs));
    ring_material_->setTexture(rc->getUnchecked<Texture>(desc.rings.texture), 0);
    ring_material_->setTexture(rc->getUnchecked<Texture>("base:textures/noise.jpg"), 1);
    ring_material_->setUniform("rings_texture", 0);
    ring_material_->setUniform("noise_texture", 1);
    ring_material_->setUniform("sun_direction", Vec3{0.0f, 0.0f, 1.0f});
    ring_material_->setStateDisable(gfx::RenderState::CullFace);
    ring_material_->setDepthWrite(false);
    ring_material_->enableAlphaBlending();
    // ring_renderable_->setRenderQueueGroup(RenderQueueGroup::Group1);

    // Update shader parameters
    ring_material_->setUniform("ring_dimensions",
                               Vec3(desc.rings.min_radius, desc.rings.max_radius,
                                    desc.rings.max_radius - desc.rings.min_radius));
    ring_material_->setUniform("planet_radius", desc.radius);
    if (desc.has_atmosphere || !math::Equal(desc.radius, desc.atmosphere_radius())) {
        ring_material_->setUniform("planet_penumbra_recip",
                                   1.0f / (desc.atmosphere_radius() - desc.radius));
    } else {
        ring_material_->setUniform("planet_penumbra_recip", 1.0f);
    }
    ring_material_->setUniform("atmosphere_colour", Vec3(1.0f, 0.0f, 0.0f));
    ring_material_->setUniform("distance_threshold",
                               Vec2(lod_distance_table_[13], lod_distance_table_[10]));

    ring_renderable_->setMaterial(ring_material_);
    ring_system_node_.data.renderable = ring_renderable_;
}

void PlanetRings::update(const Vec3& camera_position) {
    ring_material_->setUniform("camera_position", camera_position);
}

void PlanetRings::updatePositions(const Vec3& planet_position, const Vec3& sun_direction) {
    // Convert light direction into a position by moving it from the planets
    // position along the direction vector by a large number.
    // NOTE: This is somewhat hacky but it works.
    Vec3 sun_position = planet_position + sun_direction * 1e9f;
    Vec3 sun_to_planet = planet_position - sun_position;

    ring_material_->setUniform("planet_position", planet_position);
    ring_material_->setUniform("light_to_planet", Vec4(sun_to_planet.x, sun_to_planet.y,
                                                       sun_to_planet.z, sun_to_planet.Length()));
    ring_material_->setUniform("light_position", sun_position);
}

float PlanetRings::getLodDistance(uint level) const {
    assert(level < lod_distance_table_.size());
    return lod_distance_table_[level];
}

Colour PlanetRings::getColour(const Vec2& position) const {
    /*
    float distance = position.Length();
    float index = (distance - min_radius_) / (max_radius_ - min_radius_);

    // Trim the outer parts of the ring.
    if (index < 0.0f || index > 1.0f) {
        return Colour::Black;
    }

    size_t scaledIndex = static_cast<size_t>(math::Floor(index * (float)mRingTexture.getWidth()));
    Colour colour = mRingTexture.getColourAt(scaledIndex, 0, 0);
    colour.a = 1.0f;
    return colour;
     */
    return Colour{};
}

float PlanetRings::getDensity(const Vec2& position) const {
    /*
    float distance = position.Length();
    float index = (distance - min_radius_) / (max_radius_ - min_radius_);

    // Trim the outer parts of the ring.
    if (index < 0.0f || index > 1.0f) {
        return 0.0f;
    }

    size_t scaledIndex = static_cast<size_t>(math::Floor(index * (float)mRingTexture.getWidth()));
    return mRingTexture.getColourAt(scaledIndex, 0, 0).a;
     */
    return 0.0f;
}

float PlanetRings::getThickness() const {
    return 1500.0f;
}
}  // namespace dw
