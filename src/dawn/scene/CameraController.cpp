/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "scene/CameraController.h"

namespace dw {
CameraController::CameraController(Context* context, float acceleration)
    : Object{context},
      velocity_{0.0f, 0.0f, 0.0f},
      roll_velocity_{0.0f},
      acceleration_{acceleration} {
    addEventListener<MouseMoveEvent>(makeEventDelegate(this, &CameraController::onMouseMove));
}

CameraController::~CameraController() {
    removeEventListener<MouseMoveEvent>(makeEventDelegate(this, &CameraController::onMouseMove));
}

void CameraController::setAcceleration(float acceleration) {
    acceleration_ = acceleration;
}

void CameraController::possess(Entity* entity) {
    assert(entity->transform());
    possessed_ = entity;
}

Entity* CameraController::possessed() const {
    return possessed_;
}

void CameraController::update(float dt) {
    if (!possessed_) {
        return;
    }

    auto input = module<Input>();
    float forward_acceleration = acceleration_ * (static_cast<float>(input->isKeyDown(Key::S)) -
                                                  static_cast<float>(input->isKeyDown(Key::W)));
    float right_acceleration = acceleration_ * (static_cast<float>(input->isKeyDown(Key::D)) -
                                                static_cast<float>(input->isKeyDown(Key::A)));
    float roll_acceleration = 10.0f * (static_cast<float>(input->isKeyDown(Key::Q)) -
                                       static_cast<float>(input->isKeyDown(Key::E)));
    if (input->isKeyDown(Key::LeftShift) || input->isKeyDown(Key::RightShift)) {
        forward_acceleration *= 10.0f;
        right_acceleration *= 10.0f;
    }

    velocity_.x = damp(velocity_.x, 0.0f, 0.99f, dt);
    velocity_.y = damp(velocity_.y, 0.0f, 0.99f, dt);
    velocity_.z = damp(velocity_.z, 0.0f, 0.99f, dt);
    roll_velocity_ = damp(roll_velocity_, 0.0f, 0.99f, dt);
    velocity_ += possessed_->transform()->orientation() *
                 Vec3{right_acceleration, 0.0f, forward_acceleration} * dt;
    roll_velocity_ += roll_acceleration * dt;

    possessed_->transform()->orientation() =
        possessed_->transform()->orientation() * Quat::RotateZ(roll_velocity_ * dt);
    possessed_->transform()->move(velocity_ * dt);
}

void CameraController::onMouseMove(const MouseMoveEvent& m) {
    if (!possessed_) {
        return;
    }

    if (module<Input>()->isMouseButtonDown(MouseButton::Left)) {
        float units_to_radians = -0.003f;
        auto& orientation = possessed_->transform()->orientation();
        orientation = orientation * Quat::RotateX(m.offset.y * units_to_radians) *
                      Quat::RotateY(m.offset.x * units_to_radians);
        orientation.Normalize();
    }
}
}  // namespace dw
