/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Renderable.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"
#include "resource/Resource.h"
#include "renderer/rhi/Renderer.h"

namespace dw {

class DW_API Mesh : public Resource, public Renderable {
public:
    DW_OBJECT(Mesh);

    Mesh(Context* context);
    ~Mesh();

    bool beginLoad(const String& asset_name, InputStream& src) override;
    void endLoad() override;

    void draw(Renderer* renderer, uint view, detail::Transform& camera, const Mat4& model_matrix,
              const Mat4& view_projection_matrix) override;

    class Node;
    class SubMesh;

    Node* rootNode();

private:
    SharedPtr<VertexBuffer> vertex_buffer_;
    SharedPtr<IndexBuffer> index_buffer_;
    UniquePtr<Node> root_node_;
    Vector<UniquePtr<SubMesh>> submeshes_;
};

class DW_API Mesh::Node {
public:
    Node(Mat4 transform, Node* parent, Vector<SubMesh*> submeshes);

    void addChild(UniquePtr<Node> node);
    Node* child(int i);
    int childCount() const;

    void setTransform(const Mat4& transform);
    const Mat4& transform() const;

    void draw(Renderer* renderer, uint view, const Mat4& model_matrix,
              const Mat4& view_projection_matrix);

private:
    Mat4 transform_;
    Node* parent_;
    Vector<SubMesh*> submeshes_;
    Vector<UniquePtr<Node>> children_;
};

class DW_API Mesh::SubMesh {
public:
    SubMesh(u32 index_buffer_offset, u32 index_count, SharedPtr<Material> material);

    void draw(Renderer* renderer, uint view, const Mat4& model_matrix,
              const Mat4& view_projection_matrix);

private:
    u32 index_buffer_offset_;
    u32 index_count_;
    SharedPtr<Material> material_;
};
}  // namespace dw
