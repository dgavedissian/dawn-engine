/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/CustomMesh.h"

namespace dw {
class DW_API MeshBuilder : public Object {
public:
    DW_OBJECT(MeshBuilder);

    MeshBuilder(Context* context);
    ~MeshBuilder();

    MeshBuilder& withNormals(bool normals);
    MeshBuilder& withTexcoords(bool texcoords);

    SharedPtr<CustomMesh> createBox(float halfSize);
    SharedPtr<CustomMesh> createSphere(float radius, uint rings = 25, uint segments = 25);

private:
    bool with_normals_;
    bool with_texcoords_;
};
}  // namespace dw