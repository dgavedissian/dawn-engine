/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Material.h"
#include "renderer/Renderer.h"
#include "renderer/Renderable.h"

#include "scene/Component.h"
#include "scene/TransformComponent.h"

namespace dw {
class DW_API SceneNode {
public:
    SceneNode();
    SceneNode(SharedPtr<Renderable> renderable);
    SceneNode(SharedPtr<Renderable> renderable, const Vec3& position, const Quat& orientation,
              const Vec3& scale = Vec3::one);
    ~SceneNode();

    void setPosition(const Vec3& position);
    void setOrientation(const Quat& orientation);
    void setScale(const Vec3& scale);

    /// Add child.
    void addChild(SharedPtr<Renderable> renderable);

    /// Add child.
    void addChild(SharedPtr<SceneNode> child);

    /// Calls renderable_->draw(...) on renderable renders children.
    void drawSceneGraph(Renderer* renderer, uint view, TransformComponent* camera,
                        const Mat4& model_matrix, const Mat4& view_projection_matrix);

    /// Gets the renderable
    Renderable* renderable() const;

private:
    SharedPtr<Renderable> renderable_;
    Mat4 local_offset_;
    SceneNode* parent_;
    Vector<SharedPtr<SceneNode>> children_;
};

struct RenderableComponent : public Component {
    explicit RenderableComponent(SharedPtr<Renderable> r) : node{makeShared<SceneNode>(r)} {
    }
    explicit RenderableComponent(SharedPtr<SceneNode> n) : node{n} {
    }
    SharedPtr<SceneNode> node;
};
}  // namespace dw
