/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "scene/CameraController.h"

namespace dw {
CameraController::CameraController(Context* context, float acceleration, float max_speed)
    : Object{context}, acceleration_{acceleration}, max_speed_{max_speed} {
    addEventListener<MouseMoveEvent>(makeEventDelegate(this, &CameraController::onMouseMove));

    for (float& i : speed_) {
        i = 0.0f;
    }
}

CameraController::~CameraController() {
    removeEventListener<MouseMoveEvent>(makeEventDelegate(this, &CameraController::onMouseMove));
}

void CameraController::possess(Entity* entity) {
    assert(entity->transform());
    possessed_ = entity;
}

void CameraController::update(float dt) {
    if (!possessed_) {
        return;
    }

    // Change speed depending on key state.
    auto input = subsystem<Input>();
    auto direction_handler = [this, input, dt](Key::Enum k, Direction d) {
        if (input->isKeyDown(k)) {
            speed_[int(d)] = min(speed_[int(d)] + acceleration_ * dt, max_speed_);
        } else {
            // Dampen speed by 99% every second.
            speed_[int(d)] = damp(speed_[int(d)], 0.0f, 0.99f, dt);
        }
    };
    direction_handler(Key::W, Direction::Front);
    direction_handler(Key::S, Direction::Back);
    direction_handler(Key::A, Direction::Left);
    direction_handler(Key::D, Direction::Right);

    // Update position.
    Vec3 velocity{speed_[int(Direction::Right)] - speed_[int(Direction::Left)], 0.0f,
                  speed_[int(Direction::Back)] - speed_[int(Direction::Front)]};
    if (input->isKeyDown(Key::LeftShift) || input->isKeyDown(Key::RightShift)) {
        velocity *= 2.0f;
    }
    possessed_->transform()->position() += possessed_->transform()->orientation() * (velocity * dt);
}

void CameraController::onMouseMove(const MouseMoveEvent& m) {
    if (!possessed_) {
        return;
    }

    if (subsystem<Input>()->isMouseButtonDown(MouseButton::Left)) {
        float mouse_move_to_delta_angle = 0.005f;
        Quat delta = Quat::RotateX(-m.offset.y * mouse_move_to_delta_angle) *
                     Quat::RotateY(-m.offset.x * mouse_move_to_delta_angle);
        possessed_->transform()->orientation() =
            (possessed_->transform()->orientation() * delta).Normalized();
    }
}
}  // namespace dw
