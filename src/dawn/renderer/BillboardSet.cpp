/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "io/StringInputStream.h"
#include "renderer/BillboardSet.h"
#include "renderer/Program.h"
#include "renderer/Shader.h"

namespace dw {
BillboardSet::BillboardSet(Context* ctx, u32 particle_count, float particle_size)
    : Object{ctx}, particle_size_{particle_size} {
    // Shaders.
    StringInputStream vs_source{R"(
            #version 330 core

            layout(location = 0) in vec3 position;
            layout(location = 1) in vec2 texcoord;

            uniform mat4 mvp_matrix;

            out vec2 frag_texcoord;

            void main()
            {
                frag_texcoord = texcoord;
                gl_Position = mvp_matrix * vec4(position, 1.0);
            }
        )"};
    StringInputStream fs_source{R"(
            #version 330 core

            in vec2 frag_texcoord;

            uniform sampler2D billboard_texture;

            layout(location = 0) out vec4 out_colour;

            void main()
            {
                out_colour = texture(billboard_texture, frag_texcoord);
            }
        )"};
    auto vertex_shader = makeShared<VertexShader>(ctx);
    vertex_shader->load("billboard_set.vs", vs_source);
    auto fragment_shader = makeShared<FragmentShader>(ctx);
    fragment_shader->load("billboard_set.fs", fs_source);
    setMaterial(
        makeShared<Material>(ctx, makeShared<Program>(ctx, vertex_shader, fragment_shader)));
    material_->setUniform<int>("billboard_texture", 0);

    // Create vertex and index buffers.
    u32 vertex_count = particle_count * 4;
    u32 index_count = particle_count * 6;
    VertexDecl decl;
    decl.begin()
        .add(VertexDecl::Attribute::Position, 3, VertexDecl::AttributeType::Float)
        .add(VertexDecl::Attribute::TexCoord0, 2, VertexDecl::AttributeType::Float)
        .end();
    vb_ = makeShared<VertexBuffer>(ctx, nullptr, vertex_count * sizeof(ParticleVertex),
                                   vertex_count, decl, BufferUsage::Dynamic);
    ib_ = makeShared<IndexBuffer>(ctx, nullptr, index_count * sizeof(u32), IndexBufferType::U32);

    // Initialise data stores.
    resize(particle_count);
}

void BillboardSet::resize(u32 particle_count) {
    particles_.resize(particle_count);
    for (int i = 0; i < particle_count; ++i) {
        particles_[i].position = Vec3(0.0f, 0.0f, 0.0f);
        particles_[i].size = Vec2(particle_size_, particle_size_);
    }

    // Allocate vertex data.
    vertex_data_.reserve(particle_count * 4);

    // Build index data.
    index_data_.resize(particle_count * 6);
    for (int i = 0; i < particle_count; ++i) {
        // 0 1
        // 2 3
        u32 start_vertex = static_cast<u32>(i) * 4;
        index_data_[i * 6 + 0] = start_vertex;
        index_data_[i * 6 + 1] = start_vertex + 3;
        index_data_[i * 6 + 2] = start_vertex + 1;
        index_data_[i * 6 + 3] = start_vertex;
        index_data_[i * 6 + 4] = start_vertex + 2;
        index_data_[i * 6 + 5] = start_vertex + 3;
    }
    ib_->update(index_data_.data(), index_data_.size() * sizeof(u32), 0);
}

void BillboardSet::setParticlePosition(int particle_id, const Vec3& position) {
    assert(particle_id < particles_.size());
    particles_[particle_id].position = position;
}

void BillboardSet::setParticleSize(int particle_id, const Vec2& size) {
    assert(particle_id < particles_.size());
    particles_[particle_id].size = size;
}

void BillboardSet::draw(Renderer* renderer, uint view, Transform* camera, const Mat4&,
                        const Mat4& view_projection_matrix) {
    update(camera);

    renderer->setVertexBuffer(vb_->internalHandle());
    renderer->setIndexBuffer(ib_->internalHandle());
    renderer->setStateEnable(RenderState::Blending);
    renderer->setStateBlendEquation(BlendEquation::Add, BlendFunc::SrcAlpha,
                                    BlendFunc::OneMinusSrcAlpha);
    material_->setUniform("mvp_matrix", view_projection_matrix);
    material_->program()->prepareForRendering();
    renderer->submit(view, material_->program()->internalHandle(), particles_.size() * 6);
}

void BillboardSet::update(Transform* camera_transform) {
    // Sort positions by reverse depth - O(nlogn).
    Vector<ParticleData> sorted_particles{particles_};
    Position camera_position = camera_transform->position();
    std::sort(sorted_particles.begin(), sorted_particles.end(),
              [camera_position](const ParticleData& a, const ParticleData& b) {
                  // We want to order from furthest to nearest, so return true if distance of a is
                  // greater.
                  return camera_position.getRelativeTo(a.position).LengthSq() >
                         camera_position.getRelativeTo(b.position).LengthSq();
              });

    // Generate vertex data.
    for (auto& p : sorted_particles) {
        Vec3 axis_x, axis_y;
        calculateAxes(camera_transform, p.position, axis_x, axis_y);

        // x y u v
        // 0 1
        // 2 3
        Vec4 quad[] = {{-1.0f, 1.0f, 0.0f, 1.0f},
                       {1.0f, 1.0f, 1.0f, 1.0f},
                       {-1.0f, -1.0f, 0.0f, 0.0f},
                       {1.0f, -1.0f, 1.0f, 0.0f}};
        for (int i = 0; i < 4; ++i) {
            Vec3 vertex_position =
                p.position + axis_x * p.size.x * quad[i].x + axis_y * p.size.y * quad[i].y;
            vertex_data_.emplace_back(ParticleVertex{vertex_position, {quad[i].z, quad[i].w}});
        }
    }

    // Update vertex buffer.
    vb_->update(vertex_data_.data(), vertex_data_.size() * sizeof(ParticleVertex), 0);
    vertex_data_.clear();
}

void BillboardSet::calculateAxes(Transform* camera_transform, const Vec3& position, Vec3& axis_x,
                                 Vec3& axis_y) {
    Vec3 to_eye = camera_transform->position().getRelativeTo(position).Normalized();

    // Point.
    axis_x = to_eye.Cross(camera_transform->orientation() * Vec3::unitY);
    axis_y = to_eye.Cross(axis_x);
}
}  // namespace dw
