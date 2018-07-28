/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Renderer.h"
#include "renderer/Renderable.h"
#include "renderer/SceneNode.h"
#include "scene/SceneManager.h"
#include "scene/C_Transform.h"
#include "scene/PhysicsScene.h"
#include "scene/C_LinearMotion.h"
#include "net/NetTransform.h"

namespace dw {
Renderer::Renderer(Context* ctx) : Module(ctx) {
    rhi_ = makeUnique<rhi::Renderer>(ctx);
}

Renderer::~Renderer() {
}

void Renderer::updateSceneGraph() {
    // Recalculate view matrices.
    auto& cameras = camera_entity_system_->cameras;
    view_proj_matrices_per_camera_.resize(cameras.size());
    for (int i = 0; i < cameras.size(); ++i) {
        view_proj_matrices_per_camera_[i] =
            cameras[i].projection_matrix * cameras[i].scene_node->calculateViewMatrix();
    }

    // Recalculate all model matrices of all LargeSceneNodes (for each camera view).
    large_model_matrices_per_camera_.resize(cameras.size());
    render_operations_per_camera_.resize(cameras.size());
    for (int i = 0; i < cameras.size(); ++i) {
        large_model_matrices_per_camera_[i].clear();
        render_operations_per_camera_[i].clear();
    }

    // Traverse large scene nodes.
    Deque<LargeSceneNodeR*> large_nodes = {&rootNode()};
    while (!large_nodes.empty()) {
        LargeSceneNodeR* node = large_nodes.front();
        large_nodes.pop_front();
        for (int i = 0; i < node->largeChildCount(); ++i) {
            large_nodes.push_back(node->largeChild(i));
        }

        // Recalculate model matrix for each camera.
        Mat4 matrix;
        for (int i = 0; i < cameras.size(); ++i) {
            matrix = node->calculateModelMatrix(cameras[i].scene_node->position);
            large_model_matrices_per_camera_[i].insert({node, matrix});
        }

        // Add render operation if a renderable is attached.
        Renderable* renderable = node->data.renderable.get();
        if (renderable) {
            for (int i = 0; i < cameras.size(); ++i) {
                render_operations_per_camera_[i].emplace_back(detail::RenderOperation{renderable, matrix});
            }
        }

        // Update transform cache of child regular scene nodes.
        for (int i = 0; i < cameras.size(); ++i) {
            for (int j = 0; j < node->childCount(); ++j) {
                updateTransformCache(matrix, i, node->child(j), Mat4::identity, false);
            }
        }
    }

    // Update transform cache of the "special" background scene node.
    for (int i = 0; i < cameras.size(); ++i) {
        updateTransformCache(Mat4::identity, i, &rootBackgroundNode(), Mat4::identity, false);
    }
}

void Renderer::renderScene(float interpolation) {
    // Render stuff.
    auto& cameras = camera_entity_system_->cameras;
    for (int i = 0; i < cameras.size(); ++i) {
        for (auto& op : render_operations_per_camera_[i]) {
            op.renderable->draw(this, i, cameras[i].scene_node, op.model, view_proj_matrices_per_camera_[i]);
        }
    }
}

void Renderer::frame() {
    rhi_->frame();
}

rhi::Renderer* Renderer::rhi() const {
    return rhi_.get();
}

LargeSceneNodeR& Renderer::rootNode() {
    return scene_graph_.root();
}

SceneNodeR& Renderer::rootBackgroundNode() {
    return scene_graph_.backgroundNode();
}

void Renderer::setupEntitySystems(SceneManager* scene_manager) {
    // entity_renderer_ = module<SceneManager>()->addSystem<EntityRenderer>();
    camera_entity_system_ = scene_manager->addSystem<CameraEntitySystem>();
}

/*
Renderer::EntityRenderer::EntityRenderer(Context* context) : System{context} {
    supportsComponents<RenderableComponent, C_Transform>();
    executesAfter<CameraEntitySystem, NetTransformSyncSystem>();
}

void Renderer::EntityRenderer::beginProcessing() {
    world_transform_cache_.clear();
    render_operations_per_camera_.clear();
}

void Renderer::EntityRenderer::processEntity(Entity& entity, float) {
    for (auto camera : module<Renderer>()->camera_entity_system_->cameras) {
        Mat4 view = camera.transform_component->modelMatrix(LargePosition::origin).Inverted();
        Mat4 model =
            deriveTransform(entity.transform(), camera.transform_component, world_transform_cache_);
        render_operations_per_camera_.push_back([this, &entity, camera, model, view](float interpolation) {
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

void Renderer::EntityRenderer::render(float interpolation) {
    for (auto& op : render_operations_per_camera_) {
        op(interpolation);
    }
}
*/

Renderer::CameraEntitySystem::CameraEntitySystem(Context* context) : System{context} {
    supportsComponents<Camera, C_Transform>();
    executesAfter<PhysicsScene::PhysicsComponentSystem>();
}

void Renderer::CameraEntitySystem::beginProcessing() {
    cameras.clear();
}

void Renderer::CameraEntitySystem::processEntity(Entity& entity, float) {
    cameras.emplace_back(CameraState{0, entity.transform()->node.get<LargeSceneNodeR*>(),
                                     entity.component<Camera>()->projection_matrix});
}

void Renderer::updateTransformCache(Mat4 large_base_world, int camera_id, SceneNodeR* node,
                                    const Mat4& parent, bool dirty) {
    Mat4& world = world_transform_cache_[node];

    dirty |= node->dirty;
    if (dirty) {
        world = parent * node->calculateModelMatrix();
        node->dirty = false;
    }

    // Add render operation.
    Renderable* renderable = node->data.renderable.get();
    if (renderable) {
        render_operations_per_camera_[camera_id].emplace_back(
            detail::RenderOperation{renderable, large_base_world * world});
    }

    for (int i = 0; i < node->childCount(); ++i) {
        updateTransformCache(large_base_world, camera_id, node->child(i), world, dirty);
    }
}
}  // namespace dw
