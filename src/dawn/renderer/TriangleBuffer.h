/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "math/Defs.h"
#include "renderer/GLVertexBuffer.h"
#include "renderer/GLIndexBuffer.h"

namespace dw {
class DW_API TriangleBuffer : public Object {
public:
    DW_OBJECT(TriangleBuffer);

    TriangleBuffer(Context* context);
    ~TriangleBuffer();

    // Estimations.
    void estimateVertexCount(uint count);
    void estimateIndexCount(uint count);

    // Begin creating new geometry.
    void begin();

    // Compile the vertex and index arrays into GPU buffers.
    Pair<SharedPtr<GLVertexBuffer>, SharedPtr<GLIndexBuffer>> end();

    // Add a vertex.
    void position(const Vec3& p);
    void normal(const Vec3& n);
    void texcoord(const Vec2& tc);

    // Add a triangle.
    void triangle(uint v0, uint v1, uint v2);

private:
    struct Vertex {
        Vec3 position;
        Vec3 normal;
        Vec2 texCoord;
    };
    Vertex mCurrentVertex;
    Vector<Vertex> mVertices;
    Vector<uint> mIndices;
};
}  // namespace dw
