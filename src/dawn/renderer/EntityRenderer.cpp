/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/EntityRenderer.h"
#include "renderer/Renderable.h"
#include "ecs/SystemManager.h"
#include "scene/Transform.h"
#include "scene/Parent.h"

namespace dw {
namespace {
Mat4 ConvertTransform(Transform& t) {
    return Mat4::Translate(t.position.x, t.position.y, t.position.z).ToFloat4x4() *
           Mat4::FromQuat(t.orientation);
}
}  // namespace

EntityRenderer::EntityRenderer(Context* context) : System{context} {
    supportsComponents<RenderableComponent, Transform>();
    camera_entity_system_ = subsystem<SystemManager>()->addSystem<CameraEntitySystem>();
}

void EntityRenderer::processEntity(Entity& entity) {
    auto renderable = entity.component<RenderableComponent>();
    auto transform = entity.component<Transform>();
    auto parent = entity.component<Parent>();

    for (auto camera : camera_entity_system_->cameras) {
        Mat4 model = ConvertTransform(*transform);
        renderable->renderable->draw(subsystem<Renderer>(), camera.view, model,
                                     camera.view_projection_matrix);
    }
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

EntityRenderer::CameraEntitySystem::CameraEntitySystem(Context* context) : System{context} {
    supportsComponents<Camera, Transform>();
    executesAfter<EntityRenderer>();
}

void EntityRenderer::CameraEntitySystem::beginProcessing() {
    cameras.clear();
}

void EntityRenderer::CameraEntitySystem::processEntity(Entity& entity) {
    auto camera = entity.component<Camera>();
    auto transform = entity.component<Transform>();
    Mat4 view = ConvertTransform(*transform).Inverted();
    cameras.emplace_back(CameraState{0, camera->projection_matrix * view});
}
}  // namespace dw
