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
Mat4 deriveTransform(Transform* transform, Transform* camera,
                     HashMap<Transform*, Mat4>& transform_cache) {
    // If this world transform hasn't been cached yet.
    auto cached_transform = transform_cache.find(transform);
    if (cached_transform == transform_cache.end()) {
        // Calculate transform relative to parent.
        Mat4 model = transform->modelMatrix(camera->position());

        // Derive world transform recursively.
        if (transform->parent()) {
            model = deriveTransform(transform->parent(), camera, transform_cache) * model;
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

void EntityRenderer::processEntity(Entity& entity, float) {
    auto renderable = entity.component<RenderableComponent>();
    for (auto camera : camera_entity_system_->cameras) {
        Mat4 model =
            deriveTransform(entity.transform(), camera.transform_component, world_transform_cache_);
        Mat4 view = camera.transform_component->modelMatrix(Position::origin).Inverted();
        renderable->node->drawSceneGraph(subsystem<Renderer>(), camera.view,
                                         camera.transform_component, model,
                                         camera.projection_matrix * view);
    }
}

EntityRenderer::CameraEntitySystem::CameraEntitySystem(Context* context) : System{context} {
    supportsComponents<Camera, Transform>();
    executesAfter<EntityRenderer>();
}

void EntityRenderer::CameraEntitySystem::beginProcessing() {
    cameras.clear();
}

void EntityRenderer::CameraEntitySystem::processEntity(Entity& entity, float) {
    cameras.emplace_back(CameraState{0, entity.transform(), entity.component<Camera>()->projection_matrix});
}
}  // namespace dw
