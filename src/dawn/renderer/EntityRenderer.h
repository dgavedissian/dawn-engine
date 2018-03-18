/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Camera.h"
#include "renderer/Renderer.h"
#include "scene/System.h"
#include "scene/Transform.h"

namespace dw {
using RenderOperation = Function<void(float)>;

class DW_API EntityRenderer : public System {
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
    class DW_API CameraEntitySystem : public System {
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
    HashMap<Transform*, Mat4> world_transform_cache_;
	Vector<RenderOperation> render_operations_;
};
}  // namespace dw
