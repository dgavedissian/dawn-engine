/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Renderer.h"
#include "renderer/Renderable.h"
#include "renderer/Node.h"
#include "scene/SceneManager.h"
#include "scene/CTransform.h"
#include "scene/PhysicsScene.h"
#include "scene/CLinearMotion.h"
#include "net/CNetTransform.h"

namespace dw {
Renderer::Renderer(Context* ctx) : Module(ctx) {
    rhi_ = makeUnique<rhi::Renderer>(ctx);
}

Renderer::~Renderer() {
}

void Renderer::updateSceneGraph() {
    // Recalculate view-proj matrices.
    auto& cameras = camera_entity_system_->cameras;
    view_proj_matrices_per_camera_.resize(cameras.size());
    for (int i = 0; i < cameras.size(); ++i) {
        /*
        view_proj_matrices_per_camera_[i] =
            cameras[i].projection_matrix * cameras[i].scene_node->calculateViewMatrix();
        */
    }

    // Create a frame -> frame ID map.
    HashMap<Frame*, int> frame_to_frame_id;
    for (int f = 0; f < scene_graph_.frameCount(); ++f) {
        frame_to_frame_id.insert({scene_graph_.frame(f), f});
    }

    // Set up render operations array.
    render_operations_per_camera_.resize(cameras.size());
    for (int c = 0; c < cameras.size(); ++c) {
        render_operations_per_camera_[c].clear();
    }

    // Recalculate model matrices of system nodes relative to each frame.
    system_model_matrices_per_frame_.resize(scene_graph_.frameCount());
    for (int f = 0; f < scene_graph_.frameCount(); ++f) {
        system_model_matrices_per_frame_[f].clear();
    }

    Deque<SystemNode*> system_nodes = {&scene_graph_.root()};
    while (!system_nodes.empty()) {
        SystemNode* node = system_nodes.front();
        system_nodes.pop_front();
        for (int i = 0; i < node->childCount(); ++i) {
            system_nodes.push_back(node->child(i));
        }

        // Calculate model matrix for each frame.
        for (int i = 0; i < scene_graph_.frameCount(); ++i) {
            Mat4 matrix = node->calculateModelMatrix(scene_graph_.frame(i)->position());
            system_model_matrices_per_frame_[i].insert({node, matrix});
        }

        // Add render operations for each camera if a renderable is attached.
        // TODO: Culling?
        Renderable* renderable = node->data.renderable.get();
        if (renderable) {
            for (int c = 0; c < cameras.size(); ++c) {
                int f = frame_to_frame_id.at(cameras[c].scene_node->frame());
                Mat4& model_matrix = system_model_matrices_per_frame_[f][node];
                render_operations_per_camera_[c].emplace_back(
                    detail::RenderOperation{renderable, model_matrix});
            }
        }
    }

    // Render each frame.
    // TODO: Each camera should probably only render the frame they're contained within.
    for (int f = 0; f < scene_graph_.frameCount(); ++f) {
        auto* frame = scene_graph_.frame(f);
        renderTree(frame->root_frame_node_.get(),
                   system_model_matrices_per_frame_[f].at(frame->system_node_), Mat4::identity,
                   false);
    }

    // Update transform cache of the "special" background scene node.
    /*
    for (int i = 0; i < cameras.size(); ++i) {
        renderTree(Mat4::identity, i, &rootBackgroundNode(), Mat4::identity, false);
    }
    */
}

void Renderer::renderScene(float interpolation) {
    // Render stuff.
    auto& cameras = camera_entity_system_->cameras;
    for (int i = 0; i < cameras.size(); ++i) {
        // All rendering is done relative to the cameras own frame. Get transform within the frame.
        Mat4& camera_model_matrix = model_matrix_cache_.at(cameras[i].scene_node);
        Mat4 view_matrix = camera_model_matrix.Inverted();
        auto camera_transform = detail::Transform::fromMat4(camera_model_matrix);

        // Process all render operations.
        for (auto& op : render_operations_per_camera_[i]) {
            op.renderable->draw(this, i, camera_transform, op.model,
                                cameras[i].projection_matrix * view_matrix);
        }
    }
}

void Renderer::frame() {
    rhi_->frame();
}

rhi::Renderer* Renderer::rhi() const {
    return rhi_.get();
}

SceneGraph& Renderer::sceneGraph() {
    return scene_graph_;
}

SystemNode& Renderer::rootNode() {
    return scene_graph_.root();
}

Node& Renderer::rootBackgroundNode() {
    return scene_graph_.backgroundNode();
}

void Renderer::setupEntitySystems(SceneManager* scene_manager) {
    // entity_renderer_ = module<SceneManager>()->addSystem<EntityRenderer>();
    camera_entity_system_ = scene_manager->addSystem<SCamera>();
}

Renderer::SCamera::SCamera(Context* context) : EntitySystem{context} {
    supportsComponents<CCamera, CTransform>();
    executesAfter<PhysicsScene::PhysicsComponentSystem>();
}

void Renderer::SCamera::beginProcessing() {
    cameras.clear();
}

void Renderer::SCamera::processEntity(Entity& entity, float) {
    cameras.emplace_back(CameraState{0, entity.component<CTransform>()->node,
                                     entity.component<CCamera>()->projection_matrix});
}

void Renderer::renderTree(Node* node, const Mat4& frame_model_matrix,
                          const Mat4& parent_model_matrix, bool dirty) {
    auto& cameras = camera_entity_system_->cameras;
    Mat4& model_matrix = model_matrix_cache_[node];

    // Update model matrix if node is dirty.
    dirty |= node->dirty;
    if (dirty) {
        model_matrix = parent_model_matrix * node->calculateModelMatrix();
        node->dirty = false;
    }

    // Add render operation for each camera.
    Renderable* renderable = node->data.renderable.get();
    if (renderable) {
        for (int c = 0; c < cameras.size(); ++c) {
            render_operations_per_camera_[c].emplace_back(
                detail::RenderOperation{renderable, frame_model_matrix * model_matrix});
        }
    }

    for (int i = 0; i < node->childCount(); ++i) {
        renderTree(node->child(i), frame_model_matrix, model_matrix, dirty);
    }
}
}  // namespace dw
