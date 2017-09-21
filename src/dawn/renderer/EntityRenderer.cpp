/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/EntityRenderer.h"
#include "renderer/Renderable.h"
#include "ecs/EntityManager.h"
#include "ecs/SystemManager.h"
#include "scene/Transform.h"

namespace dw {
namespace {
Mat4 convertTransform(Transform* t) {
    return Mat4::Translate(t->position().getRelativeTo(Position::origin)).ToFloat4x4() *
           Mat4::FromQuat(t->orientation());
}

Mat4 deriveTransform(Transform* transform, HashMap<Transform*, Mat4>& transform_cache) {
    // If this world transform hasn't been cached yet.
    auto cached_transform = transform_cache.find(transform);
    if (cached_transform == transform_cache.end()) {
        // Calculate transform relative to parent.
        Mat4 model = convertTransform(transform);

        // Derive world transform recursively.
        if (transform->parent()) {
            model = deriveTransform(transform->parent(), transform_cache) * model;
        }

        // Save to cache.
        transform_cache.insert({transform, model});
        return model;
    }
    return cached_transform->second;
}
}  // namespace

EntityRenderer::EntityRenderer(Context* context) : System{context} {
    supportsComponents<RenderableComponent, Transform>();
    camera_entity_system_ = subsystem<SystemManager>()->addSystem<CameraEntitySystem>();
}

void EntityRenderer::beginProcessing() {
    world_transform_cache_.clear();
}

void EntityRenderer::processEntity(Entity& entity) {
    auto renderable = entity.component<RenderableComponent>();
    Mat4 model = deriveTransform(entity.transform(), world_transform_cache_);
    for (auto camera : camera_entity_system_->cameras) {
        renderable->renderable->draw(subsystem<Renderer>(), camera.view, model,
                                     camera.view_projection_matrix);
    }
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
    Mat4 view = convertTransform(transform).Inverted();
    cameras.emplace_back(CameraState{0, camera->projection_matrix * view});
}
}  // namespace dw
