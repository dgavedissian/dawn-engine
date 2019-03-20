/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Node.h"
#include "renderer/RenderPipeline.h"
#include "scene/SceneManager.h"

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
    void renderScene(float dt, float interpolation);
    void renderSceneFromCamera(float dt, float interpolation, u32 camera_id, uint view, u32 mask);

    // Frames.
    Frame* addFrame(SystemNode* frame_node);
    void removeFrame(Frame* frame);
    Frame* frame(usize i);
    usize frameCount() const;

    // Callbacks. All take delta time (dt) as first parameter.
    Function<void(float)> preRenderSceneCallback;
    Function<void(float)> postRenderSceneCallback;
    Function<void(float, const detail::Transform& camera_transform, const Mat4& view_matrix,
                  const Mat4& proj_matrix)>
        preRenderCameraCallback;

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

struct CCamera;

class SceneGraph::SCamera : public EntitySystem<CCamera, CSceneNode> {
public:
    SCamera();
    ~SCamera() = default;

    void process(SceneManager* sceneManager, float dt) override;

    struct CameraState {
        uint view;
        Node* scene_node;
        Mat4 projection_matrix;
    };

    Vector<CameraState> cameras;
};
}  // namespace dw
