/*
* Dawn Engine
* Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
*/
#include "Common.h"
#include "renderer/ManualMesh.h"
#include "renderer/TriangleBuffer.h"
#include "renderer/MeshBuilder.h"

namespace dw {
MeshBuilder::MeshBuilder(Context* context)
    : Object(context), mWithNormals(false), mWithTexcoords(false) {
}

MeshBuilder::~MeshBuilder() {
}

MeshBuilder& MeshBuilder::withNormals(bool normals) {
    mWithNormals = normals;
    return *this;
}

MeshBuilder& MeshBuilder::withTexcoords(bool texcoords) {
    mWithTexcoords = texcoords;
    return *this;
}

SharedPtr<ManualMesh> MeshBuilder::createSphere(float mRadius, uint mNumRings, uint mNumSegments) {
    TriangleBuffer buffer{mContext};

    buffer.begin();
    buffer.estimateVertexCount((mNumRings + 1) * (mNumSegments + 1));
    buffer.estimateIndexCount(mNumRings * (mNumSegments + 1) * 6);

    float fDeltaRingAngle = (math::pi / mNumRings);
    float fDeltaSegAngle = (math::pi * 2.0f / mNumSegments);
    int offset = 0;

    // Generate the group of rings for the sphere.
    for (uint ring = 0; ring <= mNumRings; ring++) {
        float r0 = mRadius * sinf(ring * fDeltaRingAngle);
        float y0 = mRadius * cosf(ring * fDeltaRingAngle);

        // Generate the group of segments for the current ring.
        for (uint seg = 0; seg <= mNumSegments; seg++) {
            float x0 = r0 * sinf(seg * fDeltaSegAngle);
            float z0 = r0 * cosf(seg * fDeltaSegAngle);

            // Add one vertex to the strip which makes up the sphere.
            buffer.position(Vec3(x0, y0, z0));
            if (mWithNormals) {
                buffer.normal(Vec3(x0, y0, z0));
            }
            if (mWithTexcoords) {
                buffer.texcoord(
                    Vec2((float)seg / (float)mNumSegments, (float)ring / (float)mNumRings));
            }

            if (ring != mNumRings) {
                if (seg != mNumSegments) {
                    // Each vertex (except the last) has six indices pointing to it.
                    if (ring != mNumRings - 1) {
                        buffer.triangle(offset + mNumSegments + 2, offset,
                                        offset + mNumSegments + 1);
                    }
                    if (ring != 0) {
                        buffer.triangle(offset + mNumSegments + 2, offset + 1, offset);
                    }
                }
                offset++;
            }
        };
    }

    // Generate the mesh.
    auto builtMesh = buffer.end();
    return makeShared<ManualMesh>(mContext, builtMesh.first, builtMesh.second);
}
}