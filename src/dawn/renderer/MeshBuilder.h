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

    MeshBuilder& normals(bool normals);
    MeshBuilder& texcoords(bool texcoords);

    SharedPtr<CustomMesh> createPlane(float width, float height);
    SharedPtr<CustomMesh> createBox(float half_size);
    SharedPtr<CustomMesh> createSphere(float radius, uint rings = 25, uint segments = 25);

private:
    bool with_normals_;
    bool with_texcoords_;
};
}  // namespace dw