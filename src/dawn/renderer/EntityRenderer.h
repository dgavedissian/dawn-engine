/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Renderer.h"
#include "ecs/System.h"

namespace dw {
class DW_API EntityRenderer : public System {
public:
    DW_OBJECT(EntityRenderer);

    EntityRenderer(Context* context);
    ~EntityRenderer() = default;

    void processEntity(Entity& entity) override;

    void dispatchRenderTasks();

private:
    //HashMap<String, Vector<RenderTask>> render_tasks_by_camera_;
};
}  // namespace dw
