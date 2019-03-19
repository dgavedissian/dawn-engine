/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "renderer/SceneGraph.h"
#include "renderer/SystemPosition.h"
#include "scene/SceneManager.h"
#include "scene/PhysicsScene.h"
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
    HashMap<Frame*, usize> frame_to_frame_id;
    for (usize f = 0; f < frameCount(); ++f) {
        frame_to_frame_id.insert({frame(f), f});
    }

    // Set up render operations array.
    render_operations_per_camera_.resize(cameras.size());
    for (usize c = 0; c < cameras.size(); ++c) {
        render_operations_per_camera_[c].clear();
    }

    // Render the background node.
    for (usize c = 0; c < cameras.size(); ++c) {
        const auto* camera_node = cameras[c].scene_node;
        auto background_transform = Mat4::Translate(camera_node->transform().position).ToFloat4x4();
        renderTree(&background_root_, background_transform, Mat4::identity, false, c);
    }

    // Recalculate model matrices of system nodes relative to each frame.
    system_model_matrices_per_frame_.resize(frameCount());
    for (usize f = 0; f < frameCount(); ++f) {
        system_model_matrices_per_frame_[f].clear();
    }

    Deque<SystemNode*> system_nodes = {&root_};
    while (!system_nodes.empty()) {
        SystemNode* node = system_nodes.front();
        system_nodes.pop_front();
        for (usize i = 0; i < node->childCount(); ++i) {
            system_nodes.push_back(node->child(i));
        }

        // Calculate model matrix for each frame.
        for (usize i = 0; i < frameCount(); ++i) {
            Mat4 matrix = node->calculateModelMatrix(frame(i)->position());
            system_model_matrices_per_frame_[i].insert({node, matrix});
        }

        // Add render operations for each camera if a renderable is attached.
        // TODO: Culling?
        Renderable* renderable = node->data.renderable.get();
        if (renderable) {
            for (usize c = 0; c < cameras.size(); ++c) {
                usize f = frame_to_frame_id.at(cameras[c].scene_node->frame());
                Mat4& model_matrix = system_model_matrices_per_frame_[f][node];
                render_operations_per_camera_[c].emplace_back(
                    detail::RenderOperation{renderable, model_matrix});
            }
        }
    }

    // Render each frame.
    // TODO: Each camera should probably only render the frame they're contained within.
    for (usize f = 0; f < frameCount(); ++f) {
        auto* fr = frame(f);
        renderTree(fr->root_frame_node_.get(),
                   system_model_matrices_per_frame_[f].at(fr->system_node_), Mat4::identity, false,
                   -1);
    }
}

void SceneGraph::renderScene(float dt, float interpolation) {
    if (preRenderSceneCallback) {
        preRenderSceneCallback(dt);
    }
    if (!camera_entity_system_->cameras.empty()) {
        render_pipeline_->render(dt, interpolation, this, 0);
    }
    if (postRenderSceneCallback) {
        postRenderSceneCallback(dt);
    }
}

void SceneGraph::renderSceneFromCamera(float dt, float, u32 camera_id, uint view, u32 mask) {
    // Render stuff.
    auto& cameras = camera_entity_system_->cameras;
    assert(camera_id < cameras.size());

    // All rendering is done relative to the cameras own frame. Get transform within the frame.
    const Mat4& camera_model_matrix = model_matrix_cache_.at(cameras[camera_id].scene_node);

    // Calculate matrices.
    const Mat4 view_matrix = camera_model_matrix.Inverted();
    const Mat4& proj_matrix = cameras[camera_id].projection_matrix;
    const Mat4 view_proj_matrix = proj_matrix * view_matrix;
    auto camera_transform = detail::Transform::fromMat4(camera_model_matrix);

    // Process all render operations.
    if (preRenderCameraCallback) {
        preRenderCameraCallback(dt, camera_transform, view_matrix, proj_matrix);
    }
    for (auto& op : render_operations_per_camera_[camera_id]) {
        if (op.renderable->material()->mask() & mask) {
            op.renderable->draw(module<Renderer>(), view, camera_transform, op.model,
                                view_proj_matrix);
        }
    }
}

Frame* SceneGraph::addFrame(SystemNode* frame_node) {
    frames_.emplace_back(makeUnique<Frame>(frame_node));
    return frames_.back().get();
}

void SceneGraph::removeFrame(Frame* frame) {
    (void)std::remove_if(frames_.begin(), frames_.end(), [frame](const UniquePtr<Frame>& element) {
        return frame == element.get();
    });
}

Frame* SceneGraph::frame(usize i) {
    return frames_[i].get();
}

usize SceneGraph::frameCount() const {
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
            for (usize c = 0; c < cameras.size(); ++c) {
                render_operations_per_camera_[c].emplace_back(
                    detail::RenderOperation{renderable, frame_model_matrix * model_matrix});
            }
        } else {
            render_operations_per_camera_[camera_id].emplace_back(
                detail::RenderOperation{renderable, frame_model_matrix * model_matrix});
        }
    }

    for (usize i = 0; i < node->childCount(); ++i) {
        renderTree(node->child(i), frame_model_matrix, model_matrix, dirty, camera_id);
    }
}

SceneGraph::SCamera::SCamera() {
    executesAfter<PhysicsScene::PhysicsComponentSystem>();
}

void SceneGraph::SCamera::process(SceneManager* sceneManager, float) {
    cameras.clear();
    for (auto e: view(sceneManager)) {
        auto entity = Entity{sceneManager, e};
        cameras.emplace_back(CameraState{0, entity.component<CTransform>()->node,
                                         entity.component<CCamera>()->projection_matrix});
    }
}
}  // namespace dw
