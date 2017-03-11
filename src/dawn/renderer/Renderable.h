/*
* Dawn Engine
* Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
*/
#pragma once

#include "renderer/Material.h"
#include "ecs/Component.h"
#include "renderer/RenderTask.h"

namespace dw {
class DW_API Renderable : public Object, public Component {
public:
    DW_OBJECT(Renderable);

    Renderable(Context* context);
    virtual ~Renderable();

    /// @brief Returns the material of this Renderable.
    /// @return The material currently assigned to this Renderable.
    Material* material() const;

    /// @brief Changes the material used to render this Renderable object.
    /// @param material The material to assign to this Renderable.
    void setMaterial(SharedPtr<Material> material);

    /// @brief Generates a render task.
    /// @return A generated render task for this draw event.
    virtual RenderTask draw(const Mat4& modelMatrix) = 0;

protected:
    SharedPtr<Material> material_;
};
}
