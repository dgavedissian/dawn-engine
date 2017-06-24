/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"
#include "renderer/TriangleBuffer.h"

namespace dw {
TriangleBuffer::TriangleBuffer(Context* context) : Object(context) {
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
}

Pair<SharedPtr<VertexBuffer>, SharedPtr<IndexBuffer>> TriangleBuffer::end() {
    SharedPtr<VertexBuffer> vb;
    SharedPtr<IndexBuffer> ib;
    // TODO.
    return {vb, ib};
}

void TriangleBuffer::position(const Vec3& p) {
    vertices_.emplace_back(current_vertex_);
    current_vertex_ = Vertex{};
    current_vertex_.position = p;
}

void TriangleBuffer::normal(const Vec3& n) {
    current_vertex_.normal = n;
}

void TriangleBuffer::texcoord(const Vec2& tc) {
    current_vertex_.texCoord = tc;
}

void TriangleBuffer::triangle(uint v0, uint v1, uint v2) {
    indices_.emplace_back(v0);
    indices_.emplace_back(v1);
    indices_.emplace_back(v2);
}
}  // namespace dw
