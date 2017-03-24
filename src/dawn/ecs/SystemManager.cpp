/*
* Dawn Engine
* Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
*/
#include "Common.h"
#include "ecs/SystemManager.h"

namespace dw {
SystemManager::SystemManager(Context* context)
    : Object{context}, system_manager_{context->ontology_world_.getSystemManager()} {
}

void SystemManager::update() {
    context_->ontology_world_.update();
}
}
