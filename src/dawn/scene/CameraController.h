/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "input/Input.h"
#include "ecs/Entity.h"

namespace dw {
class DW_API CameraController : public Object {
public:
    DW_OBJECT(CameraController);

    CameraController(Context* context, float acceleration, float max_speed);
    ~CameraController();

    void possess(Entity* entity);

    void update(float dt);

private:
    Entity* possessed_;

    enum class Direction { Front = 0, Back, Left, Right };
    float speed_[4];
    float acceleration_;
    float max_speed_;

    void onMouseMove(const MouseMoveEvent& m);
};
}  // namespace dw