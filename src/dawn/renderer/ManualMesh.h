/*
* Dawn Engine
* Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
*/
#pragma once

#include "renderer/Geometry.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"

namespace dw {
class DW_API ManualMesh : public Object, public Geometry {
public:
    DW_OBJECT(ManualMesh);

    ManualMesh(Context* context, SharedPtr<VertexBuffer> vertexBuffer,
               SharedPtr<IndexBuffer> indexBuffer);
    ~ManualMesh();

    void draw(SharedPtr<Material> material) override;

private:
    SharedPtr<VertexBuffer> mVertexBuffer;
    SharedPtr<IndexBuffer> mIndexBuffer;
};
}
