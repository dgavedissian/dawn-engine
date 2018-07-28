/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Camera.h"
#include "renderer/rhi/Renderer.h"
#include "scene/System.h"
#include "core/scene/SceneGraph.h"
#include "renderer/Renderable.h"

namespace dw {
namespace detail {
struct RenderOperation {
    Renderable* renderable;
    Mat4 model;
};
}  // namespace detail
// using RenderOperation = Function<void(float)>;

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

    /// Get the root scene node.
    LargeSceneNodeR& rootNode();

    /// Get the root camera scene node.
    SceneNodeR& rootBackgroundNode();

    void setupEntitySystems(SceneManager* scene_manager);

private:
    UniquePtr<rhi::Renderer> rhi_;

    SceneGraph<detail::RendererSceneNodeData> scene_graph_;

    /*
    class EntityRenderer : public System {
    public:
        DW_OBJECT(EntityRenderer);

        EntityRenderer(Context* context);
        ~EntityRenderer() = default;

        void beginProcessing() override;
        void processEntity(Entity& entity, float dt) override;

        // Called during rendering, with an interpolation factor used to extrapolate the last state
        // of the world.
        void render(float interpolation);

    private:
        HashMap<C_Transform*, Mat4> world_transform_cache_;
        Vector<RenderOperation> render_operations_per_camera_;
    };
    EntityRenderer* entity_renderer_;
    */

    class CameraEntitySystem : public System {
    public:
        DW_OBJECT(CameraEntitySystem);

        CameraEntitySystem(Context* context);
        ~CameraEntitySystem() = default;

        void beginProcessing() override;
        void processEntity(Entity& entity, float dt) override;

        struct CameraState {
            uint view;
            LargeSceneNodeR* scene_node;
            Mat4 projection_matrix;
        };

        Vector<CameraState> cameras;
    };

    CameraEntitySystem* camera_entity_system_;

    Vector<Mat4> view_proj_matrices_per_camera_;
    Vector<HashMap<LargeSceneNodeR*, Mat4>> large_model_matrices_per_camera_;
    HashMap<SceneNodeR*, Mat4> world_transform_cache_;
    Vector<Vector<detail::RenderOperation>> render_operations_per_camera_;

    void updateTransformCache(Mat4 large_base_world, int camera_id, SceneNodeR* node,
                              const Mat4& parent, bool dirty);
};
}  // namespace dw
