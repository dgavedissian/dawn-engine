/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/SceneGraph.h"
#include "renderer/SystemPosition.h"
#include "scene/SceneManager.h"
#include "renderer/Renderer.h"
#include "renderer/CCamera.h"
#include "renderer/Renderable.h"

namespace dw {
SceneGraph::SceneGraph(Context* ctx)
    : Object(ctx),
      root_(&pool_, SystemPosition::origin, Quat::identity),
      background_root_(&pool_, nullptr, Vec3::zero, Quat::identity, Vec3::one),
      camera_entity_system_(nullptr) {
    auto default_render_pipeline_node = RenderPipelineDesc::Node{
        {},
        {{"out", rhi::TextureFormat::RGBA8}},
        {RenderPipelineDesc::ClearStep{}, RenderPipelineDesc::RenderQueueStep{}}};
    auto default_render_pipeline =
        RenderPipelineDesc{{{"Default", default_render_pipeline_node}},
                           {},
                           {RenderPipelineDesc::NodeInstance{
                               "Default", {}, {{"out", RenderPipelineDesc::PipelineOutput}}}}};
    setRenderPipeline(*RenderPipeline::createFromDesc(ctx, default_render_pipeline));
    assert(render_pipeline_);
}

SceneGraph::~SceneGraph() {
}

void SceneGraph::setupEntitySystems(SceneManager* scene_manager) {
    camera_entity_system_ = scene_manager->addSystem<SCamera>();
}

void SceneGraph::setRenderPipeline(SharedPtr<RenderPipeline> render_pipeline) {
    render_pipeline_ = render_pipeline;
}

void SceneGraph::updateSceneGraph() {
    auto& cameras = camera_entity_system_->cameras;

    // Create a frame -> frame ID map.
    HashMap<Frame*, int> frame_to_frame_id;
    for (int f = 0; f < frameCount(); ++f) {
        frame_to_frame_id.insert({frame(f), f});
    }

    // Set up render operations array.
    render_operations_per_camera_.resize(cameras.size());
    for (int c = 0; c < cameras.size(); ++c) {
        render_operations_per_camera_[c].clear();
    }

    // Render the background node.
    for (int c = 0; c < cameras.size(); ++c) {
        const auto* camera_node = cameras[c].scene_node;
        auto background_transform = Mat4::Translate(camera_node->transform().position).ToFloat4x4();
        renderTree(&background_root_, background_transform, Mat4::identity, false, c);
    }

    // Recalculate model matrices of system nodes relative to each frame.
    system_model_matrices_per_frame_.resize(frameCount());
    for (int f = 0; f < frameCount(); ++f) {
        system_model_matrices_per_frame_[f].clear();
    }

    Deque<SystemNode*> system_nodes = {&root_};
    while (!system_nodes.empty()) {
        SystemNode* node = system_nodes.front();
        system_nodes.pop_front();
        for (int i = 0; i < node->childCount(); ++i) {
            system_nodes.push_back(node->child(i));
        }

        // Calculate model matrix for each frame.
        for (int i = 0; i < frameCount(); ++i) {
            Mat4 matrix = node->calculateModelMatrix(frame(i)->position());
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
    for (int f = 0; f < frameCount(); ++f) {
        auto* fr = frame(f);
        renderTree(fr->root_frame_node_.get(),
                   system_model_matrices_per_frame_[f].at(fr->system_node_), Mat4::identity, false,
                   -1);
    }
}

void SceneGraph::renderScene(float interpolation) {
    if (camera_entity_system_->cameras.size() > 0) {
        render_pipeline_->render(interpolation, this, 0);
    }
}

void SceneGraph::renderSceneFromCamera(float, u32 camera_id, uint view, u32 mask) {
    // Render stuff.
    auto& cameras = camera_entity_system_->cameras;
    assert(camera_id < cameras.size());

    // All rendering is done relative to the cameras own frame. Get transform within the frame.
    Mat4& camera_model_matrix = model_matrix_cache_.at(cameras[camera_id].scene_node);
    Mat4 view_matrix = camera_model_matrix.Inverted();
    auto camera_transform = detail::Transform::fromMat4(camera_model_matrix);

    // Process all render operations.
    for (auto& op : render_operations_per_camera_[camera_id]) {
        op.renderable->draw(module<Renderer>(), view, camera_transform, op.model,
                            cameras[camera_id].projection_matrix * view_matrix);
    }
}

Frame* SceneGraph::addFrame(SystemNode* frame_node) {
    frames_.emplace_back(makeUnique<Frame>(frame_node));
    return frames_.back().get();
}

void SceneGraph::removeFrame(Frame* frame) {
    std::remove_if(frames_.begin(), frames_.end(),
                   [frame](const UniquePtr<Frame>& element) { return frame == element.get(); });
}

Frame* SceneGraph::frame(int i) {
    return frames_[i].get();
}

int SceneGraph::frameCount() const {
    return frames_.size();
}

SystemNode& SceneGraph::root() {
    return root_;
}

Node& SceneGraph::backgroundNode() {
    return background_root_;
}

void SceneGraph::renderTree(Node* node, const Mat4& frame_model_matrix,
                            const Mat4& parent_model_matrix, bool dirty, int camera_id) {
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
        if (camera_id == -1) {
            auto& cameras = camera_entity_system_->cameras;
            for (int c = 0; c < cameras.size(); ++c) {
                render_operations_per_camera_[c].emplace_back(
                    detail::RenderOperation{renderable, frame_model_matrix * model_matrix});
            }
        } else {
            render_operations_per_camera_[camera_id].emplace_back(
                detail::RenderOperation{renderable, frame_model_matrix * model_matrix});
        }
    }

    for (int i = 0; i < node->childCount(); ++i) {
        renderTree(node->child(i), frame_model_matrix, model_matrix, dirty, camera_id);
    }
}

SceneGraph::SCamera::SCamera(Context* context) : EntitySystem{context} {
    supportsComponents<CCamera, CTransform>();
    executesAfter<PhysicsScene::PhysicsComponentSystem>();
}

void SceneGraph::SCamera::beginProcessing() {
    cameras.clear();
}

void SceneGraph::SCamera::processEntity(Entity& entity, float) {
    cameras.emplace_back(CameraState{0, entity.component<CTransform>()->node,
                                     entity.component<CCamera>()->projection_matrix});
}
}  // namespace dw
