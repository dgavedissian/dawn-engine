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
    mVertices.reserve(count);
}

void TriangleBuffer::estimateIndexCount(uint count) {
    mIndices.reserve(count);
}

void TriangleBuffer::begin() {
    mVertices.clear();
    mIndices.clear();
}

Pair<SharedPtr<VertexBuffer>, SharedPtr<IndexBuffer>> TriangleBuffer::end() {
    SharedPtr<VertexBuffer> vb;
    SharedPtr<IndexBuffer> ib;
    // TODO.
    return {vb, ib};
}

void TriangleBuffer::position(const Vec3& p) {
    mVertices.emplace_back(mCurrentVertex);
    mCurrentVertex = Vertex{};
    mCurrentVertex.position = p;
}

void TriangleBuffer::normal(const Vec3& n) {
    mCurrentVertex.normal = n;
}

void TriangleBuffer::texcoord(const Vec2& tc) {
    mCurrentVertex.texCoord = tc;
}

void TriangleBuffer::triangle(uint v0, uint v1, uint v2) {
    mIndices.emplace_back(v0);
    mIndices.emplace_back(v1);
    mIndices.emplace_back(v2);
}
}  // namespace dw
