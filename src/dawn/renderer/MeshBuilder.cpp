/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/CustomMesh.h"
#include "renderer/TriangleBuffer.h"
#include "renderer/MeshBuilder.h"

namespace dw {
MeshBuilder::MeshBuilder(Context* context)
    : Object{context}, with_normals_{false}, with_texcoords_{false} {
}

MeshBuilder::~MeshBuilder() {
}

MeshBuilder& MeshBuilder::withNormals(bool normals) {
    with_normals_ = normals;
    return *this;
}

MeshBuilder& MeshBuilder::withTexcoords(bool texcoords) {
    with_texcoords_ = texcoords;
    return *this;
}

SharedPtr<CustomMesh> MeshBuilder::createBox(float halfSize) {
    // clang-format off
    float vertices[] = {
        // Position						| Normals		      | UVs
        -halfSize, -halfSize, -halfSize, 0.0f,  0.0f,  -1.0f, 0.0f, 0.0f,
        halfSize,  -halfSize, -halfSize, 0.0f,  0.0f,  -1.0f, 1.0f, 0.0f,
        halfSize,  halfSize,  -halfSize, 0.0f,  0.0f,  -1.0f, 1.0f, 1.0f,
        halfSize,  halfSize,  -halfSize, 0.0f,  0.0f,  -1.0f, 1.0f, 1.0f,
        -halfSize, halfSize,  -halfSize, 0.0f,  0.0f,  -1.0f, 0.0f, 1.0f,
        -halfSize, -halfSize, -halfSize, 0.0f,  0.0f,  -1.0f, 0.0f, 0.0f,

        -halfSize, -halfSize, halfSize,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
        halfSize,  -halfSize, halfSize,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
        halfSize,  halfSize,  halfSize,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        halfSize,  halfSize,  halfSize,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -halfSize, halfSize,  halfSize,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -halfSize, -halfSize, halfSize,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        -halfSize, halfSize,  halfSize,  -1.0f, 0.0f,  0.0f,  0.0f, 0.0f,
        -halfSize, halfSize,  -halfSize, -1.0f, 0.0f,  0.0f,  1.0f, 0.0f,
        -halfSize, -halfSize, -halfSize, -1.0f, 0.0f,  0.0f,  1.0f, 1.0f,
        -halfSize, -halfSize, -halfSize, -1.0f, 0.0f,  0.0f,  1.0f, 1.0f,
        -halfSize, -halfSize, halfSize,  -1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
        -halfSize, halfSize,  halfSize,  -1.0f, 0.0f,  0.0f,  0.0f, 0.0f,

        halfSize,  halfSize,  halfSize,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        halfSize,  halfSize,  -halfSize, 1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        halfSize,  -halfSize, -halfSize, 1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        halfSize,  -halfSize, -halfSize, 1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        halfSize,  -halfSize, halfSize,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        halfSize,  halfSize,  halfSize,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,

        -halfSize, -halfSize, -halfSize, 0.0f,  -1.0f, 0.0f,  0.0f, 0.0f,
        halfSize,  -halfSize, -halfSize, 0.0f,  -1.0f, 0.0f,  1.0f, 0.0f,
        halfSize,  -halfSize, halfSize,  0.0f,  -1.0f, 0.0f,  1.0f, 1.0f,
        halfSize,  -halfSize, halfSize,  0.0f,  -1.0f, 0.0f,  1.0f, 1.0f,
        -halfSize, -halfSize, halfSize,  0.0f,  -1.0f, 0.0f,  0.0f, 1.0f,
        -halfSize, -halfSize, -halfSize, 0.0f,  -1.0f, 0.0f,  0.0f, 0.0f,

        -halfSize, halfSize,  -halfSize, 0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        halfSize,  halfSize,  -halfSize, 0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        halfSize,  halfSize,  halfSize,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        halfSize,  halfSize,  halfSize,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        -halfSize, halfSize,  halfSize,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        -halfSize, halfSize,  -halfSize, 0.0f,  1.0f,  0.0f,  0.0f, 0.0f };
    // clang-format on

    // Build mesh.
    TriangleBuffer buffer{context_};
    buffer.begin();
    buffer.estimateVertexCount(36);
    buffer.estimateIndexCount(36);
    for (int tri = 0; tri < 12; ++tri) {
        for (int v = 0; v < 3; ++v) {
            float* data = &vertices[(tri * 3 + v) * 8];
            buffer.position({data[0], data[1], data[2]});
            if (with_normals_) {
                buffer.normal({data[3], data[4], data[5]});
            }
            if (with_texcoords_) {
                buffer.texcoord({data[6], data[7]});
            }
        }
        buffer.triangle(tri * 3, tri * 3 + 1, tri * 3 + 2);
    }
    return buffer.end();
}

SharedPtr<CustomMesh> MeshBuilder::createSphere(float radius, uint num_rings, uint num_segments) {
    TriangleBuffer buffer{context_};

    buffer.begin();
    buffer.estimateVertexCount((num_rings + 1) * (num_segments + 1));
    buffer.estimateIndexCount(num_rings * (num_segments + 1) * 6);

    float delta_ring_angle = (math::pi / num_rings);
    float delta_seg_angle = (math::pi * 2.0f / num_segments);
    uint offset = 0;

    // Generate the group of rings for the sphere.
    for (uint ring = 0; ring <= num_rings; ring++) {
        float r0 = radius * sinf(ring * delta_ring_angle);
        float y0 = radius * cosf(ring * delta_ring_angle);

        // Generate the group of segments for the current ring.
        for (uint seg = 0; seg <= num_segments; seg++) {
            float x0 = r0 * sinf(seg * delta_seg_angle);
            float z0 = r0 * cosf(seg * delta_seg_angle);

            // Add one vertex to the strip which makes up the sphere.
            buffer.position({x0, y0, z0});
            if (with_normals_) {
                buffer.normal({x0, y0, z0});
            }
            if (with_texcoords_) {
                buffer.texcoord({static_cast<float>(seg) / static_cast<float>(num_segments),
                                 static_cast<float>(ring) / static_cast<float>(num_rings)});
            }

            if (ring != num_rings) {
                if (seg != num_segments) {
                    // Each vertex (except the last) has six indices pointing to it.
                    if (ring != num_rings - 1) {
                        buffer.triangle(offset + num_segments + 2, offset,
                                        offset + num_segments + 1);
                    }
                    if (ring != 0) {
                        buffer.triangle(offset + num_segments + 2, offset + 1, offset);
                    }
                }
                offset++;
            }
        };
    }

    // Generate the mesh.
    return buffer.end();
}
}  // namespace dw
