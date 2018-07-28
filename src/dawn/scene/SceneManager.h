/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "renderer/SceneNode.h"
#include "scene/Entity.h"
#include "scene/C_Transform.h"

#include <ontology/World.hpp>
#include <ontology/SystemManager.hpp>

#include "scene/System.h"
#include "scene/PhysicsScene.h"

namespace dw {
/// Manages the current game world, including all entities and entity systems.
class DW_API SceneManager : public Module {
public:
    DW_OBJECT(SceneManager);

    SceneManager(Context* context);
    ~SceneManager();

    // TODO: Move this into Universe class.
    /// Set up star system.
    void createStarSystem();

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

    /// Creates a new empty entity.
    /// @param type Entity type ID.
    /// @return A newly created entity.
    Entity& createEntity(EntityType type);

    /// Creates a new entity with a transform component.
    /// @param type Entity type ID.
    /// @param p Initial position.
    /// @param o Initial orientation.
    /// @param renderable Renderable attach to this entity. Can be null.
    /// @param parent Parent entity. Can be null.
    /// @return A newly created entity.
    Entity& createEntity(EntityType type, const LargePosition& p, const Quat& o,
                         SharedPtr<Renderable> renderable = nullptr, Entity* parent = nullptr);

    /// Creates a new empty entity with a previously reserved entity ID.
    /// @param type Entity type ID.
    /// @param reserved_entity_id Entity ID reserved by 'reserveEntityId()'.
    /// @return A newly created entity.
    Entity& createEntity(EntityType type, EntityId reserved_entity_id);

    /// Reserve a new entity ID.
    /// @return Unique unused entity ID.
    EntityId reserveEntityId();

    /// Looks up an entity by its ID.
    /// @param id Entity ID.
    /// @return The entity which corresponds to this entity ID.
    Entity* findEntity(EntityId id);

    /// Removes an entity from the engine.
    /// @param entity Entity to remove.
    void removeEntity(Entity* entity);

    /// Begin main loop. Required by Ontology.
    void beginMainLoop();

    /// Updates systems, and calls update on each entity.
    /// @param dt Time elapsed
    void update(float dt);

    /// Returns the physics scene.
    PhysicsScene* physicsScene() const;

    /// Returns the last delta time. Used only by the Ontology wrapper.
    float lastDeltaTime_internal() const;

private:
    float last_dt_;
    Ontology::World ontology_world_;
    HashMap<EntityId, UniquePtr<Entity>> entity_lookup_table_;
    EntityId entity_id_allocator_;

    SceneNodeR* background_scene_node_;

    UniquePtr<PhysicsScene> physics_scene_;
};

template <typename T, typename... Args> T* SceneManager::addSystem(Args... args) {
    auto system = makeUnique<T>(context(), std::forward(args)...);
    return ontology_world_.getSystemManager()
        .addSystem<OntologySystemAdapter<T>>(std::move(system))
        .system();
}

template <typename T> T* SceneManager::getSystem() {
    return ontology_world_.getSystemManager().getSystem<OntologySystemAdapter<T>>().system();
}

template <typename T> void SceneManager::removeSystem() {
    ontology_world_.getSystemManager().removeSystem<OntologySystemAdapter<T>>();
}
}  // namespace dw
