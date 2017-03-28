/*
* Dawn Engine
* Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
*/
#pragma once

#include <ontology/SystemManager.hpp>

#include "ecs/System.h"

namespace dw {
class SystemManager : public Object {
public:
    DW_OBJECT(SystemManager);

    SystemManager(Context* context);
    virtual ~SystemManager() = default;

    /// Constructs a new entity system of the specified type.
    /// @tparam T Entity system type.
    /// @tparam Args List of constructor argument types.
    /// @param args Constructor arguments.
    template <typename T, typename... Args> T* addSystem(Args... args);

    /// Looks up an entity system in the context.
    /// @tparam T Entity system type.
    /// @return Instance of the entity system type.
    template <typename T> T* getSystem();

    /// Removes the entity system from the context.
    /// @tparam T Entity system type.
    template <typename T> void removeSystem();

    /// Update the world.
    void update();

private:
    Ontology::SystemManager& system_manager_;
};

template <typename T, typename... Args> T* SystemManager::addSystem(Args... args) {
    auto system = makeUnique<T>(context(), std::forward(args)...);
    return system_manager_.addSystem<OntologySystemAdapter<T>>(std::move(system)).system();
}

template <typename T> T* SystemManager::getSystem() {
    return system_manager_.getSystem<OntologySystemAdapter<T>>().system();
}

template <typename T> void SystemManager::removeSystem() {
    system_manager_.removeSystem<OntologySystemAdapter<T>>();
}
}
