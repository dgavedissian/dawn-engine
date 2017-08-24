/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"
#include "renderer/TriangleBuffer.h"

namespace dw {
TriangleBuffer::TriangleBuffer(Context* context)
    : Object{context}, contains_normals_{false}, contains_texcoords_{false} {
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
    const void* data;
    uint size;
    VertexDecl decl;
    if (contains_normals_ && contains_texcoords_) {
        data = vertices_.data();
        size = static_cast<uint>(vertices_.size()) * sizeof(Vertex);
        decl.begin()
            .add(VertexDecl::Attribute::Position, 3, VertexDecl::AttributeType::Float)
            .add(VertexDecl::Attribute::Normal, 3, VertexDecl::AttributeType::Float)
            .add(VertexDecl::Attribute::TexCoord0, 2, VertexDecl::AttributeType::Float)
            .end();
    } else {
        // Build VertexDecl.
        decl.begin();
        decl.add(VertexDecl::Attribute::Position, 3, VertexDecl::AttributeType::Float);
        if (contains_normals_) {
            decl.add(VertexDecl::Attribute::Normal, 3, VertexDecl::AttributeType::Float, true);
        }
        if (contains_texcoords_) {
            decl.add(VertexDecl::Attribute::TexCoord0, 2, VertexDecl::AttributeType::Float);
        }
        decl.end();

        // Build packed buffer based on parameters.
        size = vertices_.size() * decl.stride();
        uint stride =
            decl.stride() / sizeof(float);  // convert stride in bytes to stride in floats.
        float* packed_data = new float[vertices_.size() * decl.stride()];
        for (int i = 0; i < vertices_.size(); i++) {
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
        data = packed_data;
    }

    // Create custom mesh.
    auto custom_mesh = makeShared<CustomMeshRenderable>(
        context_, makeShared<VertexBuffer>(context_, data, size, vertices_.size(), decl),
        makeShared<IndexBuffer>(context_, indices_.data(), indices_.size() * sizeof(u32),
                                IndexBufferType::U32));

    // Delete packed buffer.
    if (!contains_normals_ || !contains_texcoords_) {
        delete[]((const float*)data);
    }

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
