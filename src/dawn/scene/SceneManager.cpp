/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "renderer/Renderable.h"
#include "scene/SceneManager.h"
#include "renderer/MeshBuilder.h"
#include "resource/ResourceCache.h"
#include "renderer/Renderer.h"
#include "scene/PhysicsScene.h"
#include "SLinearMotion.h"
#include "net/CNetTransform.h"
#include "renderer/SceneGraph.h"
#include "SceneManager.h"

namespace dw {
SceneManager::SceneManager(Context* ctx, EventSystem* event_system, SceneGraph* scene_graph)
    : Object(ctx), background_scene_node_(nullptr), system_process_order_dirty_(false) {
    background_scene_node_ = scene_graph->backgroundNode().newChild();

    physics_scene_ = makeUnique<PhysicsScene>(ctx, this, event_system);

    // Set up built in entity systems.
    scene_graph->setupEntitySystems(this);
    addSystem<SLinearMotion>();
    addSystem<SNetTransformSync>();
}

SceneManager::~SceneManager() {
    physics_scene_.reset();
}

void SceneManager::createStarSystem() {
    auto vs = *module<ResourceCache>()->get<VertexShader>("base:space/skybox.vs");
    auto fs = *module<ResourceCache>()->get<FragmentShader>("base:space/skybox_starfield.fs");
    auto background_material =
        makeShared<Material>(context(), makeShared<Program>(context(), vs, fs));
    background_material->setTexture(
        *module<ResourceCache>()->get<Texture>("base:space/starfield.jpg"));
    background_material->setUniform<int>("starfield_sampler", 0);
    background_material->setDepthWrite(false);
    auto skybox = MeshBuilder{context()}.normals(false).texcoords(true).createBox(-100.0f);
    skybox->setMaterial(background_material);
    background_scene_node_->data.renderable = skybox;
}

Result<void> SceneManager::recomputeSystemExecutionOrder() {
    if (!system_process_order_dirty_) {
        return {};
    }

    // Implementation of Kahn's algorithm
    // https://en.wikipedia.org/wiki/Topological_sorting#Kahn's_algorithm

    system_process_order_.clear();

    // system_dependencies_ store a map of incoming edges. For example, if it stores (1,[2]),
    // (2,[3,4]), then this means that 1 depends on 2, and 2 depends on 3 and 4. This represents the
    // following graph:
    //
    //   3 --> 2 --> 1
    //         ^
    //   4 ----`

    // Build up a data structure of the graph.
    // i.e. given: (1,[2]), (2,[3,4]), generate: (2,[1]), (3,[2]), (4,[2])
    HashMap<TypeIndex, HashSet<TypeIndex>> incoming_edges = system_dependencies_;
    HashMap<TypeIndex, HashSet<TypeIndex>> outgoing_edges;
    for (auto& dependencies : incoming_edges) {
        for (auto& depends_on : dependencies.second) {
            outgoing_edges[depends_on].emplace(dependencies.first);
        }
    }

    Vector<TypeIndex> result;

    // Find all nodes with no incoming edge.
    Queue<TypeIndex> nodes;
    for (const auto& node_pair : systems_) {
        auto node = node_pair.first;
        auto depends_on = incoming_edges.find(node);
        // if this node doesn't depend on anything (it has no incoming edges), add it.
        if (depends_on == incoming_edges.end() || depends_on->second.empty()) {
            nodes.push(node);
        }
    }

    // Do topological sort.
    while (!nodes.empty()) {
        auto node = nodes.back();
        nodes.pop();
        result.push_back(node);

        // For each node m with an edge from `node` to m (all outgoing edges of node)
        // i.e. for each system m where m depends on `node`
        for (auto& m : outgoing_edges[node]) {
            // Remove edge (node -> m),
            incoming_edges[m].erase(node);
            outgoing_edges[node].erase(m);

            // If m has no other incoming edges, insert m into nodes.
            if (incoming_edges[m].empty()) {
                nodes.push(m);
            }
        }
    }

    // If there are edges left, then there's a cycle.
    for (auto& pair : incoming_edges) {
        if (!pair.second.empty()) {
            return makeError("Cycle detected in system dependencies");
        }
    }
    for (auto& pair : outgoing_edges) {
        if (!pair.second.empty()) {
            return makeError("Cycle detected in system dependencies");
        }
    }

    // Assign new order.
    system_process_order_.reserve(result.size());
    for (auto& node : result) {
        system_process_order_.emplace_back(systems_[node].get());
    }

    return {};
}

Entity& SceneManager::createEntity(EntityType type) {
    auto new_entity = registry_.create();

    // Look up slot and move new entity into it.
    entity_lookup_table_[new_entity] = makeUnique<Entity>(this, new_entity, type);
    return *entity_lookup_table_[new_entity];
}

Entity& SceneManager::createEntity(EntityType type, const Vec3& p, const Quat& o, Frame& frame,
                                   SharedPtr<Renderable> renderable) {
    Entity& e = createEntity(type);
    e.addComponent<CSceneNode>(frame.newChild(p, o), renderable);
    return e;
}

Entity& SceneManager::createEntity(EntityType type, const Vec3& p, const Quat& o, Entity& parent,
                                   SharedPtr<Renderable> renderable) {
    Entity& e = createEntity(type);
    e.addComponent<CSceneNode>(parent.component<CSceneNode>()->node->newChild(p, o), renderable);
    return e;
}

Entity* SceneManager::findEntity(EntityId id) {
    auto it = entity_lookup_table_.find(id);
    if (it != entity_lookup_table_.end()) {
        return it->second.get();
    }
    return nullptr;
}

void SceneManager::removeEntity(Entity* entity) {
    auto id = entity->id();
    // Erase from the lookup table (and delete the memory).
    entity_lookup_table_.erase(id);
    registry_.destroy(id);
}

void SceneManager::update(float dt) {
    recomputeSystemExecutionOrder();
    for (auto& s : system_process_order_) {
        s->process(dt);
    }
    physics_scene_->update(dt, nullptr);
}

PhysicsScene* SceneManager::physicsScene() const {
    return physics_scene_.get();
}

void SceneManager::addSystemDependencies(TypeIndex type, HashSet<TypeIndex> dependencies) {
    system_dependencies_.emplace(type, std::move(dependencies));
    system_process_order_dirty_ = true;
}

EntitySystemBase::EntitySystemBase() : scene_mgr_{nullptr}, depends_on_{} {
}
}  // namespace dw
