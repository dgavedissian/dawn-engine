/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "scene/space/SystemBody.h"

namespace dw {
SystemBody::SystemBody(Context* context, float radius, SystemNode& system_node)
    : Object(context),
      radius_(radius),
      system_node_(system_node),
      orbit_(nullptr),
      parent_(nullptr) {
    assert(radius >= 0.0f);
}

SystemBody& SystemBody::addSatellite(UniquePtr<SystemBody> satellite, UniquePtr<Orbit> orbit) {
    satellite->parent_ = this;
    satellite->orbit_ = std::move(orbit);

    SystemBody& satellite_ref = *satellite;
    satellites_.push_back(std::move(satellite));
    return satellite_ref;
}

void SystemBody::preRender(Frame& frame, const Mat4& view_matrix, const Mat4& proj_matrix) {
    for (const auto& satellite : satellites_) {
        satellite->preRender(frame, view_matrix, proj_matrix);
    }
}

void SystemBody::update(float dt, Frame& frame, const Vec3& camera_position) {
    for (const auto& satellite : satellites_) {
        satellite->update(dt, frame, camera_position);
    }
}

float SystemBody::radius() const {
    return radius_;
}

SystemNode& SystemBody::getSystemNode() const {
    return system_node_;
}

const Orbit& SystemBody::getOrbit() const {
    return *orbit_;
}

const SystemBody& SystemBody::getSatellite(uint index) const {
    assert(index < satellites_.size());
    return *satellites_[index];
}

const Vector<SharedPtr<SystemBody>>& SystemBody::getAllSatellites() const {
    return satellites_;
}

void SystemBody::updatePosition(double time) {
    system_node_.position = parent_ ? parent_->system_node_.position : SystemPosition::origin;
    if (orbit_) {
        system_node_.position += orbit_->calculatePosition(time);
    }
    for (const auto& satellite : satellites_) {
        satellite->updatePosition(time);
    }
}
}  // namespace dw