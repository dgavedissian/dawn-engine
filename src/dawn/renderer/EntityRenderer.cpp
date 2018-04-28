/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/EntityRenderer.h"
#include "renderer/Renderable.h"
#include "scene/SceneManager.h"
#include "scene/Transform.h"
#include "scene/PhysicsScene.h"
#include "scene/Velocity.h"
#include "net/NetTransform.h"

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
    executesAfter<CameraEntitySystem, NetTransformSyncSystem>();
    camera_entity_system_ = module<SceneManager>()->addSystem<CameraEntitySystem>();
}

void EntityRenderer::beginProcessing() {
    world_transform_cache_.clear();
    render_operations_.clear();
}

void EntityRenderer::processEntity(Entity& entity, float) {
    for (auto camera : camera_entity_system_->cameras) {
        Mat4 view = camera.transform_component->modelMatrix(Position::origin).Inverted();
        Mat4 model =
            deriveTransform(entity.transform(), camera.transform_component, world_transform_cache_);
        render_operations_.push_back([this, &entity, camera, model, view](float interpolation) {
            auto* renderable = entity.component<RenderableComponent>();
            auto* rigid_body = entity.component<RigidBody>();
            auto* velocity = entity.component<Velocity>();
            if (rigid_body) {
                // Apply velocity to model matrix.
                Vec3 velocity = rigid_body->_rigidBody()->getLinearVelocity() * interpolation;
                model.Translate(velocity * model.RotatePart());
            } else if (velocity) {
                model.Translate(velocity->velocity * interpolation);
            }
            renderable->node->drawSceneGraph(module<Renderer>(), camera.view,
                                             camera.transform_component, model,
                                             camera.projection_matrix * view);
        });
    }
}

void EntityRenderer::render(float interpolation) {
    for (auto& op : render_operations_) {
        op(interpolation);
    }
}

EntityRenderer::CameraEntitySystem::CameraEntitySystem(Context* context) : System{context} {
    supportsComponents<Camera, Transform>();
    executesAfter<PhysicsScene::PhysicsComponentSystem>();
}

void EntityRenderer::CameraEntitySystem::beginProcessing() {
    cameras.clear();
}

void EntityRenderer::CameraEntitySystem::processEntity(Entity& entity, float) {
    cameras.emplace_back(
        CameraState{0, entity.transform(), entity.component<Camera>()->projection_matrix});
}
}  // namespace dw
