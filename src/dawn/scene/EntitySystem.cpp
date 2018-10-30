/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "scene/EntitySystem.h"
#include "scene/SceneManager.h"

namespace dw {
void OntologySystemAdapterHelper::setOntologyAdapter(EntitySystem* entity_system,
                                                     Ontology::System* adapter) {
    entity_system->setOntologyAdapter(adapter);
}

float OntologySystemAdapterHelper::getDeltaTimeFromSceneManager(SceneManager* scene_manager) {
    return scene_manager->lastDeltaTime_internal();
}

EntitySystem::EntitySystem(Context* context) : Object{context}, ontology_system_{nullptr} {
}

void EntitySystem::beginProcessing() {
}

void EntitySystem::setOntologyAdapter(Ontology::System* system) {
    ontology_system_ = system;
    ontology_system_->setTypeSets(supported_components_, depending_systems_);
}
}  // namespace dw
