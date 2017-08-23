/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Renderable.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"
#include "resource/Resource.h"

namespace dw {
class DW_API Mesh : public Resource, public Renderable {
public:
    DW_OBJECT(Mesh);

    Mesh(Context* context);
    ~Mesh();

    bool beginLoad(InputStream& src) override;
    void endLoad() override;

    void draw(Renderer* renderer, uint view, const Mat4& modelMatrix) override;

private:
    SharedPtr<VertexBuffer> vertex_buffer_;
    SharedPtr<IndexBuffer> index_buffer_;
};
}  // namespace dw
