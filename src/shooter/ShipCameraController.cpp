#include "DawnEngine.h"
#include "ShipCameraController.h"

using namespace dw;

ShipCameraController::ShipCameraController(Context* ctx, const Vec3& offset)
    : Object{ctx}, possessed_{nullptr}, followed_{nullptr}, offset_{offset} {
}

void ShipCameraController::follow(const Entity* ship) {
    followed_ = ship;
}

void ShipCameraController::possess(Entity* camera) {
    possessed_ = camera;
}

void ShipCameraController::update(float dt) {
    if (!possessed_ || !followed_) {
        return;
    }
    auto* possessed_transform = possessed_->transform();
    const auto* followed_transform = followed_->transform();
    if (!possessed_transform || !followed_transform) {
        return;
    }

    possessed_transform->orientation = followed_transform->orientation;
    possessed_transform->position =
        followed_transform->position + followed_transform->orientation * offset_;

    // Follow behind.
    if (followed_->hasComponent<CRigidBody>()) {
        Vec3 velocity = followed_->component<CRigidBody>()->_rigidBody()->getLinearVelocity();
        float timestep = 1.0f / 60.0f;
        float scale = 3.0f;
        possessed_transform->position -= velocity * timestep * scale;
    }
}
