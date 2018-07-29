/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "scene/EntitySystem.h"

namespace dw {
EntitySystem::EntitySystem(Context* context) : Object{context}, ontology_system_{nullptr} {
}

void EntitySystem::beginProcessing() {
}

void EntitySystem::setOntologyAdapter_internal(Ontology::System* system) {
    ontology_system_ = system;
    ontology_system_->setTypeSets(supported_components_, depending_systems_);
}
}  // namespace dw