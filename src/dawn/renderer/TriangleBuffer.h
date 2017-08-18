/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "math/Defs.h"
#include "renderer/CustomMeshRenderable.h"

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
    SharedPtr<CustomMeshRenderable> end();

    // Add a vertex.
    void position(const Vec3& p);
    void normal(const Vec3& n);
    void texcoord(const Vec2& tc);

    // Add a triangle.
    void triangle(u32 v0, u32 v1, u32 v2);

private:
    struct Vertex {
        Vertex() : position{0.0f, 0.0f, 0.0f}, normal{0.0f, 0.0f, 0.0f}, tex_coord{0.0f, 0.0f} {
        }
        Vec3 position;
        Vec3 normal;
        Vec2 tex_coord;
    };
    bool contains_normals_;
    bool contains_texcoords_;
    Vector<Vertex> vertices_;
    Vector<u32> indices_;
};
}  // namespace dw
