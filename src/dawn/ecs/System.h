/*
* Dawn Engine
* Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
*/
#pragma once

#include "ontology/System.hpp"
#include "ecs/Entity.h"

namespace dw {
template <typename T> class OntologySystemAdapter : public Ontology::System {
public:
    OntologySystemAdapter(UniquePtr<T> wrapped_system) : wrapped_system_{wrapped_system} {
        wrapped_system_->internalSetOntologyAdapter(this);
    }

    void initialise() override {
    }

    void processEntity(Ontology::Entity& entity) override {
        Entity wrapped_entity{wrapped_system_.context(), entity};
        wrapped_system_->processEntity(wrapped_entity);
    }

    void configureEntity(Ontology::Entity&, std::string) override {
    }

    T* system() {
        return wrapped_system_.get();
    }

private:
    UniquePtr<T> wrapped_system_;
};

class DW_API System : public Object {
public:
    DW_OBJECT(System);

    System(Context* context) : Object{context}, ontology_system_{nullptr} {};
    virtual ~System() = default;

    /// @brief Specifies a product of component types which constrains which entities are
    ///        received which is processed by this system.
    /// @tparam T List of component types.
    /// @return This system.
    template <typename... T> System& supportsComponents() {
        ontology_system_->supportsComponents<T...>();
        return *this;
    }

    /// @brief Specifies a list of systems which this system depends on.
    /// @tparam T List of system types.
    /// @return This system.
    template <typename... T> System& executesAfter() {
        ontology_system_->executesAfter<T...>();
        return *this;
    }

    /// @brief Processes a single entity which matches the constraints set up by SupportsComponents.
    /// @param entity Entity to process.
    virtual void processEntity(Entity& entity) = 0;

    /// Internal.
    void internalSetOntologyAdapter(Ontology::System* system) {
        ontology_system_ = system;
    }

private:
    Ontology::System* ontology_system_;
};
}