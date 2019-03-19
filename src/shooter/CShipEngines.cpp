/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Core.h"
#include "scene/CTransform.h"
#include "net/CNetData.h"
#include "net/CNetTransform.h"
#include "Resource.h"
#include "CShipEngines.h"

using namespace dw;

ShipEngineData::ShipEngineData(const Vec3& force, const Vec3& offset)
    : activity_(0.0f), force_(force), offset_(offset) {
}

void ShipEngineData::fire(float power) {
    activity_ = power;
}

void ShipEngineData::update(float dt) {
    activity_ = damp(activity_, 0.0f, 0.99f, dt);
}

float ShipEngineData::activity() const {
    return activity_;
}

Vec3 ShipEngineData::force() const {
    return force_;
}

Vec3 ShipEngineData::offset() const {
    return offset_;
}

ShipEngineInstance::ShipEngineInstance(ShipEngineData* parent, const Vec3& force,
                                       const Vec3& offset, bool forwards)
    : force_(force), offset_(offset), parent_(parent), forwards_(forwards) {
}

Vec3 ShipEngineInstance::force() const {
    return force_;
}

Vec3 ShipEngineInstance::offset() const {
    return offset_;
}

Vec3 ShipEngineInstance::torque() const {
    // Applying a moment of force (torque) is T = r x F (r is the position, F is the force
    // vector).
    return -force_.Cross(offset_);
}

ShipEngineData* ShipEngineInstance::parent() const {
    return parent_;
}

bool ShipEngineInstance::isForwards() const {
    return forwards_;
}

CShipEngines::CShipEngines(Context* ctx, const Vector<ShipEngineData>& movement_engines,
                           const Vector<ShipEngineData>& nav_engines)
    : Object(ctx),
      engine_data_(movement_engines),
      nav_engine_data_(nav_engines),
      current_movement_power_(Vec3::zero),
      current_rotational_power_(Vec3::zero) {
    // Generate movement engines.
    Vector<Vec3> movement_axes = {
        {1.0f, 0.0f, 0.0f},  // right.
        {0.0f, 1.0f, 0.0f},  // up.
        {0.0f, 0.0f, 1.0f}   // backwards.
    };
    for (uint i = 0; i < movement_engines_.size(); ++i) {
        for (auto& engine : engine_data_) {
            Vec3 proj_force = engine.force().ProjectToNorm(movement_axes[i]);
            bool forwards = proj_force.AngleBetween(movement_axes[i]) < math::pi * 0.5f;
            movement_engines_[i].emplace_back(
                ShipEngineInstance(&engine, proj_force, Vec3::zero, forwards));
        }
    }

    // Generate navigation engines by projecting onto each rotation axis.
    Vector<Pair<String, Vec3>> rotation_axes = {
        {"yz plane (pitch - x rotation)", Vec3{1.0f, 0.0f, 0.0f}},
        {"xz plane (yaw - y rotation)", Vec3{0.0f, 1.0f, 0.0f}},
        {"xy plane (roll - z rotation)", Vec3{0.0f, 0.0f, 1.0f}}};
    for (uint i = 0; i < navigation_engines_.size(); ++i) {
        log().info("For %s", rotation_axes[i].first);
        Plane rotation_plane{rotation_axes[i].second, 0.0f};
        for (auto& engine : nav_engine_data_) {
            Vec3 proj_force = rotation_plane.Project(engine.force());
            Vec3 proj_position = rotation_plane.Project(engine.offset());
            Vec3 cross = proj_force.Cross(proj_position);

            // If the cross product is below the plane, then the direction is clockwise,
            // otherwise anticlockwise.
            float signed_distance = rotation_plane.SignedDistance(cross);
            bool positive = signed_distance < 0.0f;

            if (abs(signed_distance) > 0.01f) {
                log().info("Engine %s %s - Projected %s %s - Direction: %s (%.0f)",
                           engine.force().ToString(), engine.offset().ToString(),
                           proj_force.ToString(), proj_position.ToString(),
                           positive ? "positive" : "negative", signed_distance);

                navigation_engines_[i].emplace_back(
                    ShipEngineInstance{&engine, proj_force, proj_position, positive});
            }
        }
    }
}

void CShipEngines::onAddToEntity(Entity* parent) {
    auto* transform = parent->component<CTransform>();
    assert(transform);

    // Initialise engine particles.
    u32 total_engines = engine_data_.size() + nav_engine_data_.size();

    glow_billboards_ = makeShared<BillboardSet>(context(), total_engines, Vec2{10.0f, 10.0f});
    glow_billboards_->material()->setTexture(
        module<ResourceCache>()->get<Texture>("shooter:engine/glow.png").value(), 0);
    transform->node->newChild()->data.renderable = glow_billboards_;

    trail_billboards_ = makeShared<BillboardSet>(context(), total_engines, Vec2{10.0f, 10.0f});
    trail_billboards_->material()->setTexture(
        module<ResourceCache>()->get<Texture>("shooter:engine/trail.png").value(), 0);
    trail_billboards_->setBillboardType(BillboardType::Directional);
    transform->node->newChild()->data.renderable = trail_billboards_;
}

void CShipEngines::calculateMaxMovementForce(Vec3& pos_force, Vec3& neg_force) {
    pos_force = {0.0f, 0.0f, 0.0f};
    neg_force = {0.0f, 0.0f, 0.0f};
    for (size_t i = 0; i < movement_engines_.size(); ++i) {
        for (auto& engine : movement_engines_[i]) {
            if (engine.isForwards()) {
                pos_force += engine.force();
            } else {
                neg_force += engine.force();
            }
        }
    }
}

Vec3 CShipEngines::fireMovementEngines(const Vec3& power) {
    Vec3 total_force{0.0f, 0.0f, 0.0f};
    for (size_t i = 0; i < movement_engines_.size(); ++i) {
        bool forwards = power[i] > 0.0f;
        for (auto& engine : movement_engines_[i]) {
            if (engine.isForwards() == forwards) {
                Vec3 engine_force = engine.force() * abs(power[i]);
                total_force += engine_force;
                if (engine_force.Length() > 0.01f) {
                    engine.parent()->fire(abs(power[i]));
                }
            }
        }
    }
    current_movement_power_ = power;
    return total_force;
}

void CShipEngines::calculateMaxRotationalTorque(Vec3& clockwise, Vec3& anticlockwise) const {
    // Pitch - X axis.
    // Yaw - Y axis.
    // Roll - Z axis.
    clockwise = {0.0f, 0.0f, 0.0f};
    anticlockwise = {0.0f, 0.0f, 0.0f};
    for (size_t i = 0; i < navigation_engines_.size(); ++i) {
        for (auto& nav_engine : navigation_engines_[i]) {
            if (nav_engine.isForwards()) {
                clockwise += nav_engine.torque();
            } else {
                anticlockwise += nav_engine.torque();
            }
        }
    }
}

Vec3 CShipEngines::calculateRotationalTorque(const Vec3& power) const {
    Vec3 total_torque{0.0f, 0.0f, 0.0f};
    for (size_t i = 0; i < navigation_engines_.size(); ++i) {
        bool forwards = power[i] > 0;
        for (auto& nav_engine : navigation_engines_[i]) {
            if (nav_engine.isForwards() == forwards) {
                total_torque += nav_engine.torque() * abs(power[i]);
            }
        }
    }
    return total_torque;
}

Vec3 CShipEngines::fireRotationalEngines(const Vec3& power) {
    Vec3 total_torque{0.0f, 0.0f, 0.0f};
    for (size_t i = 0; i < navigation_engines_.size(); ++i) {
        bool forwards = power[i] > 0;
        for (auto& nav_engine : navigation_engines_[i]) {
            if (nav_engine.isForwards() == forwards) {
                Vec3 engine_torque = nav_engine.torque() * abs(power[i]);
                total_torque += engine_torque;
                if (engine_torque.Length() > 0.01f) {
                    nav_engine.parent()->fire(abs(power[i]));
                }
            }
        }
    }
    current_rotational_power_ = power;
    return total_torque;
}

Vec3 CShipEngines::convertToPower(const Vec3& force, const Vec3& max_pos_force,
                                  const Vec3& max_neg_force) {
    auto single_element_to_power = [](float force, float max_pos_force,
                                      float max_neg_force) -> float {
        if (force > 0.0f) {
            return force / max_pos_force;
        } else {
            // Keep negative.
            return force / -max_neg_force;
        }
    };
    return Vec3{
        single_element_to_power(force.x, max_pos_force.x, max_neg_force.x),
        single_element_to_power(force.y, max_pos_force.y, max_neg_force.y),
        single_element_to_power(force.z, max_pos_force.z, max_neg_force.z),
    };
}

void CShipEngines::rep_setCurrentMovementPower(const Vec3& power) {
    fireMovementEngines(power);
}

void CShipEngines::rep_setCurrentRotationalPower(const Vec3& power) {
    fireRotationalEngines(power);
}

Vec3 CShipEngines::currentMovementPower() {
    Vec3 current = current_movement_power_;
    current_movement_power_ = Vec3::zero;
    return current;
}

Vec3 CShipEngines::currentRotationalPower() {
    Vec3 current = current_rotational_power_;
    current_rotational_power_ = Vec3::zero;
    return current;
}

void SShipEngines::process(SceneManager* scene_manager, float dt) {
    for (auto e : view(scene_manager)) {
        auto entity = Entity{scene_manager, e};

        auto &transform = *entity.transform();
        auto &ship_engines = *entity.component<CShipEngines>();

        auto &engines = ship_engines.engine_data_;
        auto &nav_engines = ship_engines.nav_engine_data_;

        Mat4 model = transform.toMat4();

        // Update particles.
        if (ship_engines.glow_billboards_) {
            for (size_t i = 0; i < engines.size(); i++) {
                int particle = i;
                float engine_glow_size = 4.0f * engines[i].activity();
                ship_engines.glow_billboards_->setParticlePosition(
                        particle, Vec3{model * Vec4{engines[i].offset(), 1.0f}});
                ship_engines.glow_billboards_->setParticleSize(particle,
                                                               {engine_glow_size, engine_glow_size});
                ship_engines.trail_billboards_->setParticlePosition(
                        particle, Vec3{model * Vec4{engines[i].offset(), 1.0f}});
                ship_engines.trail_billboards_->setParticleSize(
                        particle, {engine_glow_size * 0.5f, engine_glow_size * 6.0f});
                ship_engines.trail_billboards_->setParticleDirection(
                        particle, Vec3{model * Vec4{engines[i].force().Normalized(), 0.0f}});
            }
            for (size_t i = 0; i < nav_engines.size(); i++) {
                int particle = i + engines.size();
                float engine_glow_size = 2.0f * nav_engines[i].activity();
                ship_engines.glow_billboards_->setParticlePosition(
                        particle, Vec3{model * Vec4{nav_engines[i].offset(), 1.0f}});
                ship_engines.glow_billboards_->setParticleSize(particle,
                                                               {engine_glow_size, engine_glow_size});
                ship_engines.trail_billboards_->setParticlePosition(
                        particle, Vec3{model * Vec4{nav_engines[i].offset(), 1.0f}});
                ship_engines.trail_billboards_->setParticleSize(
                        particle, {engine_glow_size * 0.25f, engine_glow_size * 3.0f});
                ship_engines.trail_billboards_->setParticleDirection(
                        particle, Vec3{model * Vec4{nav_engines[i].force().Normalized(), 0.0f}});
            }
        }

        // Attenuate engines.
        for (auto &e : engines) {
            e.update(dt);
        }
        for (auto &e : nav_engines) {
            e.update(dt);
        }
    }
}
