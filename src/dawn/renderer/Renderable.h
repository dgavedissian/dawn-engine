/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Program.h"
#include "ecs/Component.h"
#include "renderer/RenderTask.h"

namespace dw {
class DW_API Renderable : public Object {
public:
    DW_OBJECT(Renderable);

    Renderable(Context* context);
    virtual ~Renderable();

    /// Returns the material of this Renderable.
    /// @return The material currently assigned to this Renderable.
    Program* material() const;

    /// Changes the material used to render this Renderable object.
    /// @param material The material to assign to this Renderable.
    void setMaterial(SharedPtr<Program> material);

    /// Generates a render task.
    /// @return A generated render task for this draw event.
    virtual RenderTask draw(const Mat4& modelMatrix) = 0;

protected:
    SharedPtr<Program> material_;
};

struct RenderableComponent : public Component {
    RenderableComponent(Renderable* renderable) : renderable{renderable} {
    }
    Renderable* renderable;
};
}  // namespace dw
