/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Camera.h"
#include "renderer/Renderer.h"
#include "ecs/System.h"

namespace dw {
class DW_API EntityRenderer : public System {
public:
    DW_OBJECT(EntityRenderer);

    EntityRenderer(Context* context);
    ~EntityRenderer() = default;

    void beginProcessing() override;
    void processEntity(Entity& entity) override;

private:
    class DW_API CameraEntitySystem : public System {
    public:
        DW_OBJECT(CameraEntitySystem);

        CameraEntitySystem(Context* context);
        ~CameraEntitySystem() = default;

        void beginProcessing() override;
        void processEntity(Entity& entity) override;

        struct CameraState {
            uint view;
            Mat4 view_projection_matrix;
        };
        Vector<CameraState> cameras;
    };
    CameraEntitySystem* camera_entity_system_;
    HashMap<EntityId, Mat4> world_transform_cache_;
};
}  // namespace dw
