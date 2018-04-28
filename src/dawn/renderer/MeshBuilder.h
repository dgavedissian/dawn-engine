/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "renderer/CustomMeshRenderable.h"

namespace dw {
class DW_API MeshBuilder : public Object {
public:
    DW_OBJECT(MeshBuilder);

    MeshBuilder(Context* context);
    ~MeshBuilder();

    MeshBuilder& normals(bool normals);
    MeshBuilder& texcoords(bool texcoords);

    SharedPtr<CustomMeshRenderable> createPlane(float width, float height);
    SharedPtr<CustomMeshRenderable> createBox(float half_size);
    SharedPtr<CustomMeshRenderable> createSphere(float radius, uint rings = 25, uint segments = 25);

private:
    bool with_normals_;
    bool with_texcoords_;
};
}  // namespace dw