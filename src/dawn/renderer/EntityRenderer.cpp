/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/EntityRenderer.h"
#include "renderer/Renderable.h"
#include "scene/Transform.h"
#include "scene/Parent.h"

namespace dw {
EntityRenderer::EntityRenderer(Context* context) : System{context} {
    supportsComponents<RenderableComponent, Transform>();
    // render_tasks_by_camera_.emplace(makePair<String, Vector<RenderTask>>("main_camera", {}));
}

void EntityRenderer::processEntity(Entity& entity) {
    auto renderable = entity.component<RenderableComponent>();
    auto transform = entity.component<Transform>();
    auto parent = entity.component<Parent>();

    // For each camera.
    /*Vec3 relative_position = PositionData::WorldToRelative(current_camera, transform.position);
    Mat4 model = CalcModelMatrix(relative_position, transform.orientation);
    if (parent) {
    model = model * DeriveWorldTransform(parent.entity);
    }
    render_tasks_by_camera_["main_camera"].emplace_back(
        renderable->renderable->draw(Mat4::identity));
    */
}

void EntityRenderer::dispatchRenderTasks() {
    /*
    auto& renderer = *subsystem<Renderer>();
    for (auto render_tasks_list : render_tasks_by_camera_) {
        // Setup camera matrices.
        RenderTask setup_camera;
        setup_camera.type = RenderTaskType::SetCameraMatrices;
        setup_camera.camera = {math::TranslateOp(0.0f, 0.0f, -20.0f).ToFloat4x4().Inverted(),
                               Mat4::OpenGLPerspProjLH(0.1f, 1000.0f, 60.0f, 60.0f)};
        // renderer.pushRenderTask(std::move(setup_camera));

        // Push render tasks for this camera.
        for (auto render_task : render_tasks_list.second) {
            // renderer.pushRenderTask(std::move(render_task));
        }
        render_tasks_list.second.clear();
    }
     */
}
}  // namespace dw
