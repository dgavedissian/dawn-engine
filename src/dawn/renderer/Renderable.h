/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Material.h"
#include "ecs/Component.h"
#include "scene/Transform.h"

namespace dw {
class DW_API Renderable {
public:
    Renderable();
    virtual ~Renderable();

    /// Returns the material of this Renderable.
    /// @return The material currently assigned to this Renderable.
    Material* material() const;

    /// Changes the material used to render this Renderable object.
    /// @param material The material to assign to this Renderable.
    void setMaterial(SharedPtr<Material> material);

    /// Draws this renderable to the specified view.
    virtual void draw(Renderer* renderer, uint view, Transform* camera, const Mat4& model_matrix,
                      const Mat4& view_projection_matrix) = 0;

protected:
    SharedPtr<Material> material_;
};

class DW_API RenderableNode {
public:
    RenderableNode();
    RenderableNode(SharedPtr<Renderable> renderable);
    RenderableNode(SharedPtr<Renderable> renderable, const Vec3& position, const Quat& orientation);
    ~RenderableNode();

    void setPosition(const Vec3& position);
    void setOrientation(const Quat& orientation);

    /// Add child.
    void addChild(SharedPtr<Renderable> renderable);

    /// Add child.
    void addChild(SharedPtr<RenderableNode> child);

    /// Calls renderable_->draw(...) on renderable renders children.
    void drawSceneGraph(Renderer* renderer, uint view, Transform* camera, const Mat4& model_matrix,
                        const Mat4& view_projection_matrix);

    /// Gets the renderable
    Renderable* renderable() const;

private:
    SharedPtr<Renderable> renderable_;
    Mat4 local_offset_;
    RenderableNode* parent_;
    Vector<SharedPtr<RenderableNode>> children_;
};

struct RenderableComponent : public Component {
    explicit RenderableComponent(SharedPtr<Renderable> r) : node{makeShared<RenderableNode>(r)} {
    }
    explicit RenderableComponent(SharedPtr<RenderableNode> n) : node{n} {
    }
    SharedPtr<RenderableNode> node;
};
}  // namespace dw
