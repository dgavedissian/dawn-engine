/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Node.h"
#include "scene/Entity.h"
#include "scene/CTransform.h"

#include <entt/entt.hpp>

namespace dw {
class SceneGraph;
class PhysicsScene;
class EntitySystemBase;

/// Manages the current game world, including all entities and entity systems.
class DW_API SceneManager : public Object {
public:
    DW_OBJECT(SceneManager);

    SceneManager(Context* context, EventSystem* event_system, SceneGraph* scene_graph);
    ~SceneManager();

    // TODO: Move this into Universe class.
    /// Set up star system.
    void createStarSystem();

    /// Constructs a new entity system of the specified type.
    /// @tparam T Entity system type.
    /// @tparam Args List of constructor argument types.
    /// @param args Constructor arguments.
    template <typename T, typename... Args> T* addSystem(Args&&... args);

    /// Looks up an entity system in the context.
    /// @tparam T Entity system type.
    /// @return Instance of the entity system type.
    template <typename T> T* system();

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
    /// @param frame Frame to spawn the entity in.
    /// @param renderable Renderable attached to this entity. Can be null.
    /// @return A newly created entity.
    Entity& createEntity(EntityType type, const Vec3& p, const Quat& o, Frame& frame,
                         SharedPtr<Renderable> renderable = nullptr);

    /// Creates a new entity with a transform component.
    /// @param type Entity type ID.
    /// @param p Initial position.
    /// @param o Initial orientation.
    /// @param parent Parent entity.
    /// @param renderable Renderable attached to this entity. Can be null.
    /// @return A newly created entity.
    Entity& createEntity(EntityType type, const Vec3& p, const Quat& o, Entity& parent,
                         SharedPtr<Renderable> renderable = nullptr);

    /// Looks up an entity by its ID.
    /// @param id Entity ID.
    /// @return The entity which corresponds to this entity ID.
    Entity* findEntity(EntityId id);

    /// Removes an entity from the engine.
    /// @param entity Entity to remove.
    void removeEntity(Entity* entity);

    /// Updates systems, and calls update on each entity.
    /// @param dt Time elapsed
    void update(float dt);

    /// Returns the physics scene.
    PhysicsScene* physicsScene() const;

private:
    entt::basic_registry<EntityId> registry_;
    HashMap<EntityId, UniquePtr<Entity>> entity_lookup_table_;

    Node* background_scene_node_;

    UniquePtr<PhysicsScene> physics_scene_;

    Vector<UniquePtr<EntitySystemBase>> system_process_order_;

    friend class Entity;

    template <typename... T>
    friend class EntitySystem;
};

class DW_API EntitySystemBase {
public:
    EntitySystemBase() = default;
    virtual ~EntitySystemBase() = default;

    /// Process this system.
    /// @param dt Delta time.
    virtual void process(SceneManager* sceneManager, float dt) = 0;
};

template <typename... T>
class DW_API EntitySystem : public EntitySystemBase {
public:
    /// Specifies a list of systems which this system depends on.
    /// @tparam T List of system types.
    /// @return This system.
    template <typename... S> EntitySystem& executesAfter();

    /// Get a view of entities.
    entt::basic_view<EntityId, T...> view(SceneManager* sceneManager);
};

template <typename T, typename... Args> T* SceneManager::addSystem(Args&&... args) {
    auto system = makeUnique<T>(std::forward<Args>(args)...);
    auto* t = system.get();
    system_process_order_.emplace_back(std::move(system));
    return t;
}

template <typename T> T* SceneManager::system() {
    // TODO: Do this properly.
    for (auto& s : system_process_order_) {
        T* system = dynamic_cast<T*>(s.get());
        if (system) {
            return system;
        }
    }
    return nullptr;
}

template <typename T> void SceneManager::removeSystem() {
    //ontology_world_->getSystemManager().removeSystem<OntologySystemAdapter<T>>();
}

template<typename... T>
template<typename... S>
EntitySystem<T...>& EntitySystem<T...>::executesAfter() {
    /*
    if (ontology_system_) {
        ontology_system_->executesAfter<OntologySystemAdapter<T>...>();
    }
    depending_systems_ = Ontology::TypeSetGenerator<OntologySystemAdapter<T>...>();
     */
    return *this;
}

template<typename... T>
entt::basic_view<EntityId, T...> EntitySystem<T...>::view(SceneManager *sceneManager) {
    return sceneManager->registry_.view<T...>();
}
}  // namespace dw
