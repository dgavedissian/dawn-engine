/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "input/Input.h"
#include "Entity.h"

namespace dw {
class DW_API CameraController : public Object {
public:
    DW_OBJECT(CameraController);

    CameraController(Context* context, float acceleration);
    ~CameraController();

    void setAcceleration(float acceleration);

    void possess(Entity* entity);
    Entity* possessed() const;

    void update(float dt);

private:
    Entity* possessed_;

    Vec3 velocity_;
    float roll_velocity_;
    float acceleration_;

    void onMouseMove(const MouseMoveEvent& m);
};
}  // namespace dw
