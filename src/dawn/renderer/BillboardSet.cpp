/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "core/io/StringInputStream.h"
#include "renderer/BillboardSet.h"
#include "renderer/Program.h"
#include "renderer/Shader.h"
#include "renderer/Renderer.h"

namespace dw {
BillboardSet::BillboardSet(Context* ctx, u32 particle_count, const Vec2& particle_size)
    : Object{ctx}, particle_size_{particle_size}, type_{BillboardType::Point}, particle_count_{0} {
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
    material_->setStateEnable(rhi::RenderState::Blending);
    material_->setBlendEquation(rhi::BlendEquation::Add, rhi::BlendFunc::SrcAlpha,
                                rhi::BlendFunc::OneMinusSrcAlpha);
    material_->setDepthWrite(false);
    material_->setUniform<int>("billboard_texture", 0);

    // Create vertex and index buffers.
    uint vertex_count = static_cast<uint>(particle_count) * 4;
    uint index_count = static_cast<uint>(particle_count) * 6;
    rhi::VertexDecl decl;
    decl.begin()
        .add(rhi::VertexDecl::Attribute::Position, 3, rhi::VertexDecl::AttributeType::Float)
        .add(rhi::VertexDecl::Attribute::TexCoord0, 2, rhi::VertexDecl::AttributeType::Float)
        .end();
    vb_ = makeShared<VertexBuffer>(ctx, Memory(vertex_count * sizeof(ParticleVertex)), vertex_count,
                                   decl, rhi::BufferUsage::Dynamic);
    ib_ =
        makeShared<IndexBuffer>(ctx, Memory(index_count * sizeof(u32)), rhi::IndexBufferType::U32);

    // Initialise data stores.
    resize(particle_count);
}

void BillboardSet::resize(u32 particle_count) {
    particles_.resize(particle_count);
    for (u32 i = 0; i < particle_count; ++i) {
        particles_[i].visible = true;
        particles_[i].position = Vec3::zero;
        particles_[i].size = particle_size_;
        particles_[i].direction = Vec3::unitY;
    }

    // Allocate vertex data.
    vertex_data_.reserve(particle_count * 4);

    // Build index data.
    index_data_.resize(particle_count * 6);
    for (u32 i = 0; i < particle_count; ++i) {
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
    ib_->update(Memory(index_data_.data(), static_cast<uint>(index_data_.size()) * sizeof(u32)), 0);
}

void BillboardSet::setBillboardType(BillboardType type) {
    type_ = type;
}

void BillboardSet::setParticleVisible(u32 particle_id, bool visible) {
    assert(particle_id < particles_.size());
    particles_[particle_id].visible = visible;
}

void BillboardSet::setParticlePosition(u32 particle_id, const Vec3& position) {
    assert(particle_id < particles_.size());
    particles_[particle_id].position = position;
}

void BillboardSet::setParticleSize(u32 particle_id, const Vec2& size) {
    assert(particle_id < particles_.size());
    particles_[particle_id].size = size;
}

void BillboardSet::setParticleDirection(u32 particle_id, const Vec3& direction) {
    assert(particle_id < particles_.size());
    particles_[particle_id].direction = direction.Normalized();
}

void BillboardSet::draw(Renderer* renderer, uint view, detail::Transform& camera_transform,
                        const Mat4&, const Mat4& view_projection_matrix) {
    update(camera_transform);

    auto rhi = renderer->rhi();
    rhi->setVertexBuffer(vb_->internalHandle());
    rhi->setIndexBuffer(ib_->internalHandle());
    material_->applyRendererState(Mat4::identity, view_projection_matrix);
    rhi->submit(view, material_->program()->internalHandle(), particle_count_ * 6);
}

void BillboardSet::update(detail::Transform& camera_transform) {
    // Generate vertex data.
    particle_count_ = 0;
    for (auto& p : particles_) {
        if (!p.visible) {
            continue;
        }

        Vec3 axis_x, axis_y;
        calculateAxes(camera_transform, p, axis_x, axis_y);

        // x y u v
        // 0 1
        // 2 3
        Vec4 quad[] = {{-1.0f, 1.0f, 0.0f, 0.0f},
                       {1.0f, 1.0f, 1.0f, 0.0f},
                       {-1.0f, -1.0f, 0.0f, 1.0f},
                       {1.0f, -1.0f, 1.0f, 1.0f}};
        for (int i = 0; i < 4; ++i) {
            Vec3 vertex_position =
                p.position + axis_x * p.size.x * quad[i].x + axis_y * p.size.y * quad[i].y;
            vertex_data_.emplace_back(ParticleVertex{vertex_position, {quad[i].z, quad[i].w}});
        }
        particle_count_++;
    }

    // Update vertex buffer.
    vb_->update(Memory(vertex_data_), static_cast<uint>(vertex_data_.size()), 0);
    vertex_data_.clear();
}

void BillboardSet::calculateAxes(detail::Transform& camera_transform, const ParticleData& data,
                                 Vec3& axis_x, Vec3& axis_y) {
    Vec3 to_eye = (camera_transform.position - data.position).Normalized();

    // Point.
    switch (type_) {
        case BillboardType::Point:
            axis_x = to_eye.Cross(camera_transform.orientation * Vec3::unitY);
            axis_y = to_eye.Cross(axis_x);
            break;

        case BillboardType::Directional:
            axis_y = data.direction;
            axis_x = axis_y.Cross(to_eye).Normalized();
            break;
    }
}
}  // namespace dw
