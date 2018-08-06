/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "scene/Entity.h"

namespace dw {
class SceneManager;

template <typename T> class OntologySystemAdapter : public Ontology::System {
public:
    OntologySystemAdapter(UniquePtr<T>&& wrapped_system)
        : dt_{0.0f}, wrapped_system_{std::move(wrapped_system)} {
        wrapped_system_->setOntologyAdapter_internal(this);
    }

    void initialise() override {
    }

    void processEntity(Ontology::Entity& entity) override {
        if (!first_iteration) {
            wrapped_system_->beginProcessing();
            // Hack.
            dt_ = wrapped_system_->template module<SceneManager>()->lastDeltaTime_internal();
            first_iteration = true;
        }
        wrapped_system_->processEntity(*entity.getComponent<OntologyMetadata>().entity_ptr, dt_);
    }

    void configureEntity(Ontology::Entity&, std::string) override {
    }

    T* system() {
        return wrapped_system_.get();
    }

private:
    float dt_;
    UniquePtr<T> wrapped_system_;
};

class DW_API EntitySystem : public Object {
public:
    DW_OBJECT(EntitySystem)

    EntitySystem(Context* context);
    virtual ~EntitySystem() = default;

    /// Specifies a product of component types which constrains which entities are
    ///        received which is processed by this system.
    /// @tparam T List of component types.
    /// @return This system.
    template <typename... T> EntitySystem& supportsComponents() {
        if (ontology_system_) {
            ontology_system_->supportsComponents<T...>();
        }
        supported_components_ = Ontology::TypeSetGenerator<T...>();
        return *this;
    }

    /// Specifies a list of systems which this system depends on.
    /// @tparam T List of system types.
    /// @return This system.
    template <typename... T> EntitySystem& executesAfter() {
        if (ontology_system_) {
            ontology_system_->executesAfter<OntologySystemAdapter<T>...>();
        }
        depending_systems_ = Ontology::TypeSetGenerator<OntologySystemAdapter<T>...>();
        return *this;
    }

    /// Called when processing begins.
    virtual void beginProcessing();

    /// Processes a single entity which matches the constraints set up by SupportsComponents.
    /// @param entity Entity to process.
    virtual void processEntity(Entity& entity, float dt) = 0;

    /// Internal.
    void setOntologyAdapter_internal(Ontology::System* system);

private:
    Ontology::TypeSet supported_components_;
    Ontology::TypeSet depending_systems_;
    Ontology::System* ontology_system_;
};
}  // namespace dw
