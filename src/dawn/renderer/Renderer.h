/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "renderer/CCamera.h"
#include "renderer/rhi/Renderer.h"
#include "scene/EntitySystem.h"
#include "renderer/SceneGraph.h"
#include "renderer/Renderable.h"

namespace dw {
namespace detail {
struct RenderOperation {
    Renderable* renderable;
    Mat4 model;
};
}  // namespace detail

class DW_API Renderer : public Module {
public:
    DW_OBJECT(Renderer);

    Renderer(Context* ctx);
    ~Renderer();

    /// Update scene graph.
    void updateSceneGraph();

    /// Render the scene to the RHI.
    void renderScene(float interpolation);

    /// Render a single frame.
    void frame();

    /// Get the renderer hardware interface.
    rhi::Renderer* rhi() const;

    /// Get the scene graph
    SceneGraph& sceneGraph();

    /// Get the root scene node.
    SystemNode& rootNode();

    /// Get the root camera scene node.
    Node& rootBackgroundNode();

    void setupEntitySystems(SceneManager* scene_manager);

private:
    UniquePtr<rhi::Renderer> rhi_;

    SceneGraph scene_graph_;

    class SCamera;

    SCamera* camera_entity_system_;

    Vector<Mat4> view_proj_matrices_per_camera_;
    Vector<HashMap<SystemNode*, Mat4>> system_model_matrices_per_frame_;
    HashMap<Node*, Mat4> model_matrix_cache_;
    Vector<Vector<detail::RenderOperation>> render_operations_per_camera_;

    void renderTree(Node* node, const Mat4& frame_model_matrix, const Mat4& parent, bool dirty,
                    int camera_id);
};

class Renderer::SCamera : public EntitySystem {
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
