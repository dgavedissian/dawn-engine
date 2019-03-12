/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"
#include "renderer/TriangleBuffer.h"

namespace dw {
TriangleBuffer::TriangleBuffer(Context* ctx)
    : Object{ctx}, contains_normals_{false}, contains_texcoords_{false} {
}

TriangleBuffer::~TriangleBuffer() {
}

void TriangleBuffer::estimateVertexCount(uint count) {
    vertices_.reserve(count);
}

void TriangleBuffer::estimateIndexCount(uint count) {
    indices_.reserve(count);
}

void TriangleBuffer::begin() {
    vertices_.clear();
    indices_.clear();
    contains_normals_ = false;
    contains_texcoords_ = false;
}

SharedPtr<CustomMeshRenderable> TriangleBuffer::end() {
    // Set up vertex data.
    Memory data;
    rhi::VertexDecl decl;
    if (contains_normals_ && contains_texcoords_) {
        data = Memory(vertices_);
        decl.begin()
            .add(rhi::VertexDecl::Attribute::Position, 3, rhi::VertexDecl::AttributeType::Float)
            .add(rhi::VertexDecl::Attribute::Normal, 3, rhi::VertexDecl::AttributeType::Float)
            .add(rhi::VertexDecl::Attribute::TexCoord0, 2, rhi::VertexDecl::AttributeType::Float)
            .end();
    } else {
        // Build rhi::VertexDecl.
        decl.begin();
        decl.add(rhi::VertexDecl::Attribute::Position, 3, rhi::VertexDecl::AttributeType::Float);
        if (contains_normals_) {
            decl.add(rhi::VertexDecl::Attribute::Normal, 3, rhi::VertexDecl::AttributeType::Float,
                     true);
        }
        if (contains_texcoords_) {
            decl.add(rhi::VertexDecl::Attribute::TexCoord0, 2,
                     rhi::VertexDecl::AttributeType::Float);
        }
        decl.end();

        // Build packed buffer based on parameters.
        data = Memory(vertices_.size() * decl.stride());
        const uint stride =
            decl.stride() / sizeof(float);  // convert stride in bytes to stride in floats.
        auto* packed_data = reinterpret_cast<float*>(data.data());
        for (size_t i = 0; i < vertices_.size(); ++i) {
            uint offset = 0;
            Vertex& source_vertex = vertices_[i];
            float* vertex = &packed_data[i * stride];
            // Copy data.
            vertex[offset++] = source_vertex.position.x;
            vertex[offset++] = source_vertex.position.y;
            vertex[offset++] = source_vertex.position.z;
            if (contains_normals_) {
                vertex[offset++] = source_vertex.normal.x;
                vertex[offset++] = source_vertex.normal.y;
                vertex[offset++] = source_vertex.normal.z;
            }
            if (contains_texcoords_) {
                vertex[offset++] = source_vertex.tex_coord.x;
                vertex[offset++] = source_vertex.tex_coord.y;
            }
            assert(offset == stride);
        }
    }

    // Create custom mesh.
    auto custom_mesh = makeShared<CustomMeshRenderable>(
        context_,
        makeShared<VertexBuffer>(context_, std::move(data), static_cast<uint>(vertices_.size()),
                                 decl),
        makeShared<IndexBuffer>(context_, Memory(indices_), rhi::IndexBufferType::U32));

    return custom_mesh;
}

void TriangleBuffer::position(const Vec3& p) {
    vertices_.emplace_back();
    vertices_.back().position = p;
}

void TriangleBuffer::normal(const Vec3& n) {
    vertices_.back().normal = n;
    contains_normals_ = true;
}

void TriangleBuffer::texcoord(const Vec2& tc) {
    vertices_.back().tex_coord = tc;
    contains_texcoords_ = true;
}

void TriangleBuffer::triangle(u32 v0, u32 v1, u32 v2) {
    indices_.emplace_back(v0);
    indices_.emplace_back(v1);
    indices_.emplace_back(v2);
}
}  // namespace dw
