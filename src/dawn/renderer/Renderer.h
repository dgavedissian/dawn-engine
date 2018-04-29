/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Camera.h"
#include "renderer/rhi/Renderer.h"
#include "scene/System.h"
#include "scene/Transform.h"

namespace dw {
using RenderOperation = Function<void(float)>;

class DW_API Renderer : public Module {
public:
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

private:
    UniquePtr<rhi::Renderer> rhi_;

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
        HashMap<Transform*, Mat4> world_transform_cache_;
        Vector<RenderOperation> render_operations_;
    };

    class CameraEntitySystem : public System {
    public:
        DW_OBJECT(CameraEntitySystem);

        CameraEntitySystem(Context* context);
        ~CameraEntitySystem() = default;

        void beginProcessing() override;
        void processEntity(Entity& entity, float dt) override;

        struct CameraState {
            uint view;
            Transform* transform_component;
            Mat4 projection_matrix;
        };
        Vector<CameraState> cameras;
    };
    CameraEntitySystem* camera_entity_system_;

    EntityRenderer* entity_renderer_;
};
}  // namespace dw
