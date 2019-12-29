/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "renderer/Renderable.h"
#include "scene/SceneManager.h"
#include "resource/ResourceCache.h"
#include "renderer/Renderer.h"
#include "scene/PhysicsScene.h"
#include "SLinearMotion.h"
#include "net/CNetTransform.h"
#include "renderer/SceneGraph.h"
#include "SceneManager.h"
#include "renderer/CustomRenderable.h"

namespace dw {
SceneManager::SceneManager(Context* ctx, EventSystem* event_system, SceneGraph* scene_graph)
    : Object(ctx),
      background_scene_node_(nullptr),
      scene_graph_(scene_graph),
      system_process_order_dirty_(false) {
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
    auto& rc = *module<ResourceCache>();

    // Set up background.
    auto vs = rc.getUnchecked<VertexShader>("base:space/skybox.vs");
    auto fs = rc.getUnchecked<FragmentShader>("base:space/skybox_starfield.fs");
    auto background_material =
        makeShared<Material>(context(), makeShared<Program>(context(), vs, fs));
    background_material->setTexture(*rc.get<Texture>("base:space/starfield.jpg"));
    background_material->setUniform<int>("starfield_sampler", 0);
    background_material->setDepthWrite(false);
    background_material->setStateDisable(gfx::RenderState::Depth);
    background_material->setCullFrontFace(gfx::CullFrontFace::CW);
    auto skybox =
        CustomRenderable::Builder(context()).normals(false).texcoords(true).createBox(10000.0f);
    skybox->setMaterial(background_material);
    skybox->setRenderQueueGroup(RenderQueueGroup::Background);
    background_scene_node_->data.renderable = skybox;

    // Set up the galactic plane
    auto& galactic_plane_scene_node = *background_scene_node_->newChild();
    auto galactic_plane_fs =
        rc.getUnchecked<FragmentShader>("base:materials/scene/galacticPlane.fs");
    auto galactic_plane_material =
        makeShared<Material>(context(), makeShared<Program>(context(), vs, galactic_plane_fs));
    auto skysphere =
        CustomRenderable::Builder(context()).normals(false).texcoords(true).createSphere(10000.0f);
    galactic_plane_material->setTexture(*rc.get<Texture>("base:textures/scene-galactic-plane.jpg"));
    galactic_plane_material->setUniform<int>("galacticPlaneTexture", 0);
    galactic_plane_material->setDepthWrite(false);
    galactic_plane_material->setStateDisable(gfx::RenderState::Depth);
    galactic_plane_material->setCullFrontFace(gfx::CullFrontFace::CW);
    galactic_plane_material->enableAdditiveBlending();
    skysphere->setRenderQueueGroup(RenderQueueGroup::Background);
    skysphere->setMaterial(galactic_plane_material);
    galactic_plane_scene_node.data.renderable = skysphere;

    // Set up render pipeline.
    Vector<RenderPipelineDesc::Step> render_scene_steps{RenderPipelineDesc::ClearStep{},
                                                        RenderPipelineDesc::RenderQueueStep{}};
    RenderPipelineDesc::Node render_scene_node{
        {}, {{"out", gfx::TextureFormat::RGBA8}}, render_scene_steps};
    Vector<RenderPipelineDesc::NodeInstance> node_instances{
        {"RenderScene", {}, {{"out", RenderPipelineDesc::PipelineOutput}}}};
    auto rp_desc = RenderPipelineDesc{{{"RenderScene", render_scene_node}}, {}, node_instances};
    auto rp_result = RenderPipeline::createFromDesc(context(), rp_desc);
    if (!rp_result) {
        log().error("Failed to create render pipeline: {}", rp_result.error());
        std::terminate();
    }
    scene_graph_->setRenderPipeline(*rp_result);
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

    // Build an adjacency list and indegree representation of the graph from the dependencies by
    // inverting each edge. i.e. given: (1,[2]), (2,[3,4]), generate: (2,[1]), (3,[2]), (4,[2])
    HashMap<TypeIndex, HashSet<TypeIndex>> incoming_edges = system_dependencies_;
    HashMap<TypeIndex, Vector<TypeIndex>> adjacency_list;
    for (auto& dependencies : incoming_edges) {
        adjacency_list[dependencies.first] = {};
    }
    for (auto& dependencies : incoming_edges) {
        for (auto& depends_on : dependencies.second) {
            adjacency_list[depends_on].emplace_back(dependencies.first);
        }
    }
    HashMap<TypeIndex, int> indegrees;
    for (const auto& pair : adjacency_list) {
        indegrees[pair.first] = 0;
    }
    for (const auto& pair : adjacency_list) {
        for (auto dest : pair.second) {
            indegrees[dest]++;
        }
    }

    Vector<TypeIndex> result;

    // Find all nodes with no incoming edge (an in-degree of 0).
    Queue<TypeIndex> indegree0;
    for (const auto& node : indegrees) {
        if (node.second == 0) {
            indegree0.push(node.first);
        }
    }

    // Do topological sort.
    while (!indegree0.empty()) {
        // Remove a node from the indegree0 queue.
        auto node = indegree0.front();
        indegree0.pop();

        // Add to the result.
        result.push_back(node);

        // For each node m with an edge from `node` to m (in the adjacency list)
        // i.e. for each system m where m depends on `node`
        for (auto& m : adjacency_list[node]) {
            // Remove edge (node -> m),
            indegrees[m]--;

            // If m has no other incoming edges, insert m into nodes.
            if (indegrees[m] == 0) {
                indegree0.push(m);
            }
        }
    }

    // If there are edges left, then there's a cycle.
    for (auto& pair : indegrees) {
        if (pair.second > 0) {
            return makeError("Cycle detected in system dependencies");
        }
    }

    // Assign new order.
    system_process_order_.reserve(result.size());
    for (auto& node : result) {
        system_process_order_.emplace_back(systems_[node].get());
    }
    system_process_order_dirty_ = false;

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
