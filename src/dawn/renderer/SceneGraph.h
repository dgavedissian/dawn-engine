/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Node.h"
#include "renderer/RenderPipeline.h"
#include "scene/EntitySystem.h"

namespace dw {
namespace detail {
struct RenderOperation {
    Renderable* renderable;
    Mat4 model;
};
}  // namespace detail

class SceneManager;

class SceneGraph : public Object {
public:
    DW_OBJECT(SceneGraph);

    SceneGraph(Context* ctx);
    ~SceneGraph();

    void setupEntitySystems(SceneManager* scene_manager);

    // Rendering.
    void setRenderPipeline(SharedPtr<RenderPipeline> render_pipeline);
    void updateSceneGraph();
    void renderScene(float interpolation);
    void renderSceneFromCamera(float interpolation, u32 camera_id, uint view, u32 mask);

    // Frames.
    Frame* addFrame(SystemNode* frame_node);
    void removeFrame(Frame* frame);
    Frame* frame(int i);
    int frameCount() const;

    // Root system node.
    SystemNode& root();

    // Root scene node for objects attached to _all_ cameras.
    Node& backgroundNode();

private:
    detail::SceneNodePool pool_;
    SystemNode root_;
    Node background_root_;

    Vector<UniquePtr<Frame>> frames_;

    // Transient renderer settings.
    class SCamera;

    SCamera* camera_entity_system_;

    // Rendering information.
    Vector<HashMap<SystemNode*, Mat4>> system_model_matrices_per_frame_;
    HashMap<Node*, Mat4> model_matrix_cache_;
    Vector<Vector<detail::RenderOperation>> render_operations_per_camera_;

    // Render pipeline.
    SharedPtr<RenderPipeline> render_pipeline_;

    void renderTree(Node* node, const Mat4& frame_model_matrix, const Mat4& parent, bool dirty,
                    int camera_id);
};

class SceneGraph::SCamera : public EntitySystem {
public:
    DW_OBJECT(SCamera);

    SCamera(Context* context);
    ~SCamera() = default;

    void beginProcessing() override;
    void processEntity(Entity& entity, float dt) override;

    struct CameraState {
        uint view;
        Node* scene_node;
        Mat4 projection_matrix;
    };

    Vector<CameraState> cameras;
};
}  // namespace dw
