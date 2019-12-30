/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "core/TypeId.h"
#include "renderer/Node.h"
#include "scene/Entity.h"
#include "scene/CSceneNode.h"
#include "scene/space/StarSystem.h"

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
    ~SceneManager() override;

    // Set up the rendering pipeline for the game. This is not included in the constructor so unit
    // tests can avoid using the games render pipeline.
    void setupRenderPipeline();

    // TODO: Move this into Universe class.
    /// Set up star system.
    StarSystem& createStarSystem();

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

    /// Recomputes the system execution order if any systems were added or removed.
    /// This function will only do any work if any changes has happened since the
    /// last time it was called. Will be called before every tick.
    Result<void> recomputeSystemExecutionOrder();

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
    SceneGraph* scene_graph_;
    UniquePtr<StarSystem> star_system_;

    UniquePtr<PhysicsScene> physics_scene_;

    HashMap<TypeIndex, UniquePtr<EntitySystemBase>> systems_;
    // Map from a system to systems it depends on.
    HashMap<TypeIndex, HashSet<TypeIndex>> system_dependencies_;
    Vector<EntitySystemBase*> system_process_order_;
    bool system_process_order_dirty_;

    void addSystemDependencies(TypeIndex type, HashSet<TypeIndex> dependencies);

    friend class Entity;

    template <typename... T> friend class EntitySystem;
};

class DW_API EntitySystemBase {
public:
    EntitySystemBase();
    virtual ~EntitySystemBase() = default;

    /// Process this system.
    /// @param dt Delta time.
    virtual void process(float dt) = 0;

protected:
    SceneManager* scene_mgr_;
    HashSet<TypeIndex> depends_on_;

    friend class SceneManager;
};

template <typename... T> class DW_API EntitySystem : public EntitySystemBase {
public:
    /// Specifies a list of systems which this system depends on.
    /// @tparam T List of system types.
    /// @return This system.
    template <typename... S> EntitySystem& dependsOn();

    /// Get a view of entities.
    entt::basic_view<EntityId, T...> entityView();
};

template <typename T, typename... Args> T* SceneManager::addSystem(Args&&... args) {
    auto type_index = std::type_index(typeid(T));

    auto system = makeUnique<T>(std::forward<Args>(args)...);
    auto* system_ptr = system.get();
    auto& system_base = static_cast<EntitySystemBase&>(*system_ptr);

    systems_.emplace(type_index, std::move(system));
    system_base.scene_mgr_ = this;
    addSystemDependencies(type_index, system_base.depends_on_);

    return system_ptr;
}

template <typename T> T* SceneManager::system() {
    auto it = systems_.find(std::type_index(typeid(T)));
    return it != systems_.end() ? static_cast<T*>(it->second.get()) : nullptr;
}

template <typename T> void SceneManager::removeSystem() {
    auto type_index = std::type_index(typeid(T));
    systems_.erase(type_index);
    system_dependencies_.erase(type_index);
    system_process_order_dirty_ = true;
}

template <typename... T>
template <typename... S>
EntitySystem<T...>& EntitySystem<T...>::dependsOn() {
    for (auto index : {std::type_index(typeid(S))...}) {
        depends_on_.emplace(index);
    }
    // If the scene manager already exists (if we added this system already), then update it by
    // re-adding the dependencies. Otherwise, the call to addSystem will call addSystemDependencies
    // for us.
    if (scene_mgr_) {
        scene_mgr_->addSystemDependencies(std::type_index(typeid(this)), depends_on_);
    }
    return *this;
}

template <typename... T> entt::basic_view<EntityId, T...> EntitySystem<T...>::entityView() {
    return scene_mgr_->registry_.view<T...>();
}
}  // namespace dw
