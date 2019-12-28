/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "renderer/Material.h"
#include "renderer/Node.h"
#include "renderer/RenderQueue.h"

namespace dw {
class DW_API Renderer;

class DW_API Renderable {
public:
    Renderable();
    virtual ~Renderable() = default;

    /// Returns the material of this Renderable.
    /// @return The material currently assigned to this Renderable.
    Material* material() const;

    /// Changes the material used to render this Renderable object.
    /// @param material The material to assign to this Renderable.
    void setMaterial(SharedPtr<Material> material);

    /// Changes the render queue group used by this renderable.
    /// @param render_queue_group Render queue group.
    void setRenderQueueGroup(RenderQueueGroup render_queue_group);

    /// Changes the visibility flag of this renderable.
    /// Note that this node will still be processed in the scene graph, which is slower than removing it from the scene graph.
    /// @param visible Visibilty flag.
    void setVisible(bool visible = true);

    /// Draws this renderable to the specified render queue.
    virtual void draw(Renderer* renderer, uint render_queue, detail::Transform& camera,
                      const Mat4& model_matrix, const Mat4& view_projection_matrix) = 0;

    /// Returns the render queue group used by this renderable.
    RenderQueueGroup renderQueueGroup() const;

    /// Returns true if this renderable is visible.
    bool isVisible() const;

protected:
    SharedPtr<Material> material_;
    RenderQueueGroup render_queue_group_;
    bool visible_;
};
}  // namespace dw
