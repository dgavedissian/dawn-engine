/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "ecs/SystemManager.h"

namespace dw {
SystemManager::SystemManager(Context* context)
    : Subsystem{context}, system_manager_{context->ontology_world_.getSystemManager()} {
}

void SystemManager::beginMainLoop() {
    system_manager_.initialise();
}

void SystemManager::update(float dt) {
    last_dt_ = dt;
    context_->ontology_world_.update();
}

float SystemManager::_lastDt() const {
    return last_dt_;
}
}  // namespace dw
