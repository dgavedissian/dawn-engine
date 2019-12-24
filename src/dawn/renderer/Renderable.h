/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "renderer/Material.h"
#include "renderer/Node.h"

namespace dw {
class DW_API Renderer;

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
    virtual void draw(Renderer* renderer, uint view, detail::Transform& camera,
                      const Mat4& model_matrix, const Mat4& view_projection_matrix) = 0;

protected:
    SharedPtr<Material> material_;
};
}  // namespace dw
