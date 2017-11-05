/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "DawnEngine.h"
#include "ecs/EntityManager.h"
#include "ecs/Component.h"
#include "ecs/System.h"
#include "renderer/Program.h"
#include "renderer/MeshBuilder.h"
#include "resource/ResourceCache.h"
#include "scene/CameraController.h"
#include "scene/Transform.h"
#include "scene/Universe.h"
#include "renderer/BillboardSet.h"
#include "renderer/Mesh.h"
#include "ui/Imgui.h"

using namespace dw;

class Planet;

/*
 * Terrain patch:
 *
 *       0
 *   0-------1
 *   |       |
 * 3 |       | 1
 *   |       |
 *   3-------2
 *       2
 */
class PlanetTerrainPatch {
public:
    struct Vertex {
        Vec3 p;
        Vec3 n;
        Vec2 tc;
        static VertexDecl createDecl() {
            return VertexDecl{}
                .begin()
                .add(VertexDecl::Attribute::Position, 3, VertexDecl::AttributeType::Float)
                .add(VertexDecl::Attribute::Normal, 3, VertexDecl::AttributeType::Float)
                .add(VertexDecl::Attribute::TexCoord0, 2, VertexDecl::AttributeType::Float)
                .end();
        }
    };

    PlanetTerrainPatch(Planet* planet, PlanetTerrainPatch* parent, const Array<Vec3, 4>& corners,
                       int level);

    void setupAdjacentPatches(const Array<PlanetTerrainPatch*, 4>& adjacent);

    bool hasChildren() const;

    void updatePatch(const Vec3& offset);
    void generateGeometry(Vector<Vertex>& vertex_data, Vector<u32>& index_data);

private:
    Planet* planet_;
    PlanetTerrainPatch* parent_;
    Array<PlanetTerrainPatch*, 4> children_;
    Array<PlanetTerrainPatch*, 4> edge_;
    Array<Vec3, 4> corners_;
    Vec3 centre_;

    int level_;

    void split();
    void combine();

    int sharedEdgeWith(PlanetTerrainPatch* patch, int hint = 0);
};

class Planet : public Object {
public:
    DW_OBJECT(Planet);

    Planet(Context* ctx, float radius, float terrain_max_height, Entity* camera)
        : Object{ctx},
          camera_{camera},
          planet_{nullptr},
          radius_{radius},
          patch_split_distance_{radius * 12.0f},
          terrain_dirty_{false},
          noise_{0xdeadbeef, 12, 0.005f, terrain_max_height, 2.0f, 0.5f},
          run_update_thread_{true},
          terrain_patches_{},
          t_output_ready_{false} {
        auto em = subsystem<EntityManager>();
        auto rc = subsystem<ResourceCache>();

        // Set up material.
        auto material = makeShared<Material>(
            context(), makeShared<Program>(context(), rc->get<VertexShader>("base:space/planet.vs"),
                                           rc->get<FragmentShader>("base:space/planet.fs")));
        material->setTexture(rc->get<Texture>("base:space/planet.jpg"));
        material->setUniform("light_direction", Vec3{1.0f, 0.0f, 0.0f});
        material->setUniform("surface_sampler", 0);
        material->setPolygonMode(PolygonMode::Wireframe);

        // Set up renderable.
        setupTerrainRenderable();
        custom_mesh_renderable_->setMaterial(material);

        planet_ = &em->createEntity(Position::origin, Quat::identity)
                       .addComponent<RenderableComponent>(custom_mesh_renderable_);

        // Kick off terrain update thread.
        terrain_update_thread_ = Thread([this]()
        {
           while (run_update_thread_.load())
           {
               // Calculate offset and update patches.
               t_input_lock_.lock();
               Position camera_position = t_camera_position_;
               Position planet_position = t_planet_position_;
               t_input_lock_.unlock();
               updateTerrain(camera_position.getRelativeTo(planet_position));

               // If we detected a change in geometry, regenerate.
               if (terrain_dirty_) {
                   terrain_dirty_ = false;

                   LockGuard<Mutex> terrain_data_lock{t_output_lock_};
                   t_output_vertices_.clear();
                   t_output_indices_.clear();
                   generateTerrainData(t_output_vertices_, t_output_indices_);
                   t_output_ready_ = true;
               }
           }
        });
    }

    ~Planet()
    {
        run_update_thread_ = false;
        terrain_update_thread_.join();
    }

    Position& position() const {
        return planet_->transform()->position();
    }

    float radius() const {
        return radius_;
    }

    void update(float dt) {
        // Update camera position data.
        {
            LockGuard<Mutex> camera_position_lock{t_input_lock_};
            t_camera_position_ = camera_->transform()->position();
            t_planet_position_ = planet_->transform()->position();
        }

        // If we have any new terrain data ready, upload to GPU.
        if (t_output_ready_)
        {
            LockGuard<Mutex> terrain_data_lock{t_output_lock_};
            uploadTerrainDataToGpu(t_output_vertices_, t_output_indices_);
            t_output_ready_ = false;
        }
    }

private:
    Entity* camera_;

    Entity* planet_;
    float radius_;

    // Terrain mesh.
    SharedPtr<CustomMeshRenderable> custom_mesh_renderable_;

    // Bool which controls whether the update task thread is running.
    Atomic<bool> run_update_thread_;
    Thread terrain_update_thread_;

    // Update thread data.
    // INPUTS
    Position t_camera_position_;
    Position t_planet_position_;
    Mutex t_input_lock_;
    // OUTPUTS
    Vector<PlanetTerrainPatch::Vertex> t_output_vertices_;
    Vector<u32> t_output_indices_;
    Mutex t_output_lock_;
    // OUTPUT READY
    bool t_output_ready_;

    // Terrain structure data.
    Array<PlanetTerrainPatch*, 6> terrain_patches_; // Patches: +z, +x, -z, -x, +y, -y
    float patch_split_distance_;
    bool terrain_dirty_; // only used on update thread.
    fBmNoise noise_;

    PlanetTerrainPatch* allocatePatch(PlanetTerrainPatch* parent, const Array<Vec3, 4>& corners,
                                      int level) {
        return new PlanetTerrainPatch(this, parent, corners, level);
    }

    void freePatch(PlanetTerrainPatch* patch) {
        delete patch;
    }

    Vec3 calculateHeight(const Vec3 &position) {
        auto sample_position = position.Normalized() * radius_;
        double height_sample = noise_.noise(sample_position.x, sample_position.y, sample_position.z);
        return sample_position * (1.0f + height_sample / radius_);
    }

    Vec3 calculateNormal(const Vec3& position) {
        auto centre_sample_location = position.Normalized() * radius_;
        float offset = 1.0f; // 1 metre.
        float offset_radians = offset / radius_;

        // Generate 4 samples around point.
        Vec3 samples[4];
        Vec3 result_normal = Vec3::zero;
        for (int i = 0; i < 4; ++i) {
            float angle = math::pi * 0.5f * i;
            Vec3 sample_location = Vec3::FromSphericalCoordinates(centre_sample_location.ToSphericalCoordinates() + Vec3{sin(angle), cos(angle), 0.0f} * offset_radians);
            double height_sample = noise_.noise(sample_location.x, sample_location.y, sample_location.z);
            samples[i] = sample_location * (1.0f + height_sample / radius_);
        }

        // Compute normals.
        for (int i = 0; i < 4; ++i) {
            result_normal += (samples[i] - position).Cross(samples[(i + 1) % 4] - position).Normalized();
        }
        return result_normal.Normalized();
    }

    void setupTerrainRenderable() {
        // Setup renderable.
        auto vertex_decl = PlanetTerrainPatch::Vertex::createDecl();
        int default_vertex_count = 36;
        int default_index_count = 20;
        custom_mesh_renderable_ = makeShared<CustomMeshRenderable>(
            context(),
            makeShared<VertexBuffer>(context(), nullptr,
                                     default_vertex_count * vertex_decl.stride(),
                                     default_vertex_count, vertex_decl, BufferUsage::Dynamic),
            makeShared<IndexBuffer>(context(), nullptr, default_index_count * sizeof(u32),
                                    IndexBufferType::U32, BufferUsage::Dynamic));

        // Setup patches.
        float offset = math::Sqrt((radius_ * radius_) / 3.0f);
        Array<Vec3, 8> corners = {
            Vec3{-offset, offset, offset},  Vec3{offset, offset, offset},
            Vec3{offset, -offset, offset},  Vec3{-offset, -offset, offset},
            Vec3{-offset, offset, -offset}, Vec3{offset, offset, -offset},
            Vec3{offset, -offset, -offset}, Vec3{-offset, -offset, -offset},
        };
        terrain_patches_ = {
            allocatePatch(nullptr, {corners[0], corners[1], corners[2], corners[3]}, 0),
            allocatePatch(nullptr, {corners[1], corners[5], corners[6], corners[2]}, 0),
            allocatePatch(nullptr, {corners[5], corners[4], corners[7], corners[6]}, 0),
            allocatePatch(nullptr, {corners[4], corners[0], corners[3], corners[7]}, 0),
            nullptr,  // allocatePatch(nullptr, {corners[4], corners[5], corners[0], corners[1]},
                      // 0),
            nullptr   // allocatePatch(nullptr, {corners[3], corners[2], corners[6], corners[7]}, 0)
        };
        terrain_patches_[0]->setupAdjacentPatches(
            {terrain_patches_[4], terrain_patches_[1], terrain_patches_[5], terrain_patches_[3]});
        terrain_patches_[1]->setupAdjacentPatches(
            {terrain_patches_[4], terrain_patches_[2], terrain_patches_[5], terrain_patches_[0]});
        terrain_patches_[2]->setupAdjacentPatches(
            {terrain_patches_[4], terrain_patches_[3], terrain_patches_[5], terrain_patches_[1]});
        terrain_patches_[3]->setupAdjacentPatches(
            {terrain_patches_[4], terrain_patches_[0], terrain_patches_[5], terrain_patches_[2]});
        // terrain_patches_[4]->setupAdjacentPatches({terrain_patches_[2],terrain_patches_[1],terrain_patches_[0],terrain_patches_[3]});
        // terrain_patches_[5]->setupAdjacentPatches({terrain_patches_[0],terrain_patches_[1],terrain_patches_[2],terrain_patches_[3]});

        // Setup initial mesh.
        Vector<PlanetTerrainPatch::Vertex> vertex_data;
        Vector<u32> index_data;
        generateTerrainData(vertex_data, index_data);
        uploadTerrainDataToGpu(vertex_data, index_data);
    }

    void updateTerrain(const Vec3& offset)
    {
        for (auto& patch : terrain_patches_) {
            if (patch) {  // TODO: remove once patches are initialised properly.
                patch->updatePatch(offset);
            }
        }
    }

    void generateTerrainData(Vector<PlanetTerrainPatch::Vertex>& vertices, Vector<u32>& indices)
    {
        for (auto patch : terrain_patches_) {
            if (patch) {  // TODO: remove once patches are initialised properly.
                patch->generateGeometry(vertices, indices);
            }
        }
    }

    void uploadTerrainDataToGpu(const Vector<PlanetTerrainPatch::Vertex>& vertices, const Vector<u32>& indices) {
        // Upload to GPU.
        custom_mesh_renderable_->vertexBuffer()->update(
            vertices.data(), sizeof(PlanetTerrainPatch::Vertex) * vertices.size(), vertices.size(),
            0);
        custom_mesh_renderable_->indexBuffer()->update(indices.data(), sizeof(u32) * indices.size(),
                                                       0);
    }

    friend class PlanetTerrainPatch;
};

// Careful to value-initialize children_ and edge_ in the initialiser list by giving them an
// empty initializer ({}).
PlanetTerrainPatch::PlanetTerrainPatch(Planet* planet, PlanetTerrainPatch* parent,
                                       const Array<Vec3, 4>& corners, int level)
    : planet_{planet}, parent_{parent}, children_{}, edge_{}, corners_(corners), level_{level} {
    // Compute centre position.
    centre_ = Vec3::zero;
    for (auto& c : corners_) {
        centre_ += c * 0.25f;
    }
    centre_ = planet_->calculateHeight(centre_);
}

void PlanetTerrainPatch::setupAdjacentPatches(const Array<PlanetTerrainPatch*, 4>& adjacent) {
    edge_ = adjacent;
}

bool PlanetTerrainPatch::hasChildren() const {
    return children_[0] != nullptr;
}

void PlanetTerrainPatch::updatePatch(const Vec3& offset) {
    float threshold = planet_->patch_split_distance_ / math::Pow(2.0f, level_);
    threshold *= threshold;

    if (hasChildren()) {
        // Try combine.
        if (offset.DistanceSq(centre_) > threshold) {
            combine();
        } else {
            for (auto& child : children_) {
                child->updatePatch(offset);
            }
        }
    } else {
        // Try split.
        if (offset.DistanceSq(centre_) <= threshold && level_ <= 10) {
            split();
            for (auto& child : children_) {
                child->updatePatch(offset);
            }
        }
    }
}

void PlanetTerrainPatch::generateGeometry(Vector<PlanetTerrainPatch::Vertex>& vertex_data,
                                          Vector<u32>& index_data) {
    if (hasChildren()) {
        for (auto& child : children_) {
            child->generateGeometry(vertex_data, index_data);
        }
        return;
    }

    /*
     * Terrain patch geometry:
     *
     *   |   0   |
     * --0-------1---x---x
     *   | \   / |   |   |
     * 3 |   4---x-1-x---x
     *   | /   \ |   |   |
     * --3-------2---x---x
     *   |   2   |
     */
    auto vertex_start = static_cast<u32>(vertex_data.size());
    for (auto& corner : corners_) {
        Vertex v;
        v.p = corner;
        v.n = planet_->calculateNormal(v.p);
        v.tc = {0.0f, 0.0f};
        vertex_data.emplace_back(v);  // TODO: Move Vertex data into the nodes.
    }
    Vertex c;
    c.p = centre_;
    c.n = planet_->calculateNormal(c.p);
    c.tc = {0.0f, 0.0f};
    vertex_data.emplace_back(c);

    // Generate triangles.
    auto create_triangle = [&index_data, vertex_start](u32 v1, u32 v2, u32 v3) {
        index_data.emplace_back(vertex_start + v1);
        index_data.emplace_back(vertex_start + v2);
        index_data.emplace_back(vertex_start + v3);
    };
    u32 midpoint_counter = 4;
    for (u32 i = 0; i < 4; ++i) {
        if (edge_[i] && edge_[i]->hasChildren()) {
            // Get midpoint vertex and add to vertex data.
            Vertex m;
            int shared_edge = edge_[i]->sharedEdgeWith(this, (i + 2) % 4);
            m.p = edge_[i]->children_[shared_edge]->corners_[(shared_edge + 1) % 4];
            m.n = planet_->calculateNormal(m.p);
            m.tc = {0.0f, 0.0f};
            vertex_data.emplace_back(m);
            midpoint_counter++;

            // Construct two triangles.
            create_triangle(i, 4, midpoint_counter);
            create_triangle(4, (i + 1) % 4, midpoint_counter);
        } else {
            create_triangle(i, 4, (i + 1) % 4);
        }
    }
}

void PlanetTerrainPatch::split() {
    planet_->terrain_dirty_ = true;

    // Allocate child patches.
    Vec3 mid01 = (corners_[0] + corners_[1]) * 0.5f;
    Vec3 mid12 = (corners_[1] + corners_[2]) * 0.5f;
    Vec3 mid23 = (corners_[2] + corners_[3]) * 0.5f;
    Vec3 mid30 = (corners_[3] + corners_[0]) * 0.5f;
    int child_level = level_ + 1;
    children_ = {planet_->allocatePatch(this, {corners_[0], mid01, centre_, mid30}, child_level),
                 planet_->allocatePatch(this, {mid01, corners_[1], mid12, centre_}, child_level),
                 planet_->allocatePatch(this, {centre_, mid12, corners_[2], mid23}, child_level),
                 planet_->allocatePatch(this, {mid30, centre_, mid23, corners_[3]}, child_level)};

    // Setup child adjacent patches.
    for (int i = 0; i < 4; ++i) {
        auto child = children_[i];
        // Internal.
        child->edge_[(i + 1) % 4] = children_[(i + 1) % 4];
        child->edge_[(i + 2) % 4] = children_[(i + 3) % 4];
        // External.
        if (edge_[i]) {
            // Edge pointer back to us is usually offset by 2: (i + 2) mod 4.
            int edge_shared = edge_[i]->sharedEdgeWith(this, (i + 2) % 4);
            child->edge_[i] = edge_[i]->children_[(edge_shared + 1) % 4];
        }
        if (edge_[(i + 3) % 4]) {
            // Edge pointer back to us is usually offset by 2: (i + 3) + 2 mod 4 = (i + 1) mod 4.
            int edge_shared = edge_[(i + 3) % 4]->sharedEdgeWith(this, (i + 1) % 4);
            child->edge_[(i + 3) % 4] = edge_[(i + 3) % 4]->children_[edge_shared];
        }
    }

    // Recalculate height for middle points.
    Vec3 recalculated_centre = planet_->calculateHeight(centre_);
    for (int i = 0; i < 4; ++i) {
        auto child = children_[i];
        child->corners_[(i + 1) % 4] = planet_->calculateHeight(child->corners_[(i + 1) % 4]);
        child->corners_[(i + 2) % 4] = recalculated_centre;
        child->corners_[(i + 3) % 4] = planet_->calculateHeight(child->corners_[(i + 3) % 4]);
    }

    // Update adjacent child adjacent patches.
    for (int i = 0; i < 4; ++i) {
        for (int c = 0; c < 2; ++c) {
            // TODO HACK
            if (!edge_[i])
                continue;
            // TODO END HACK
            if (edge_[i]->hasChildren()) {
                int edge_with_this = edge_[i]->sharedEdgeWith(this, (i + 2) % 4);
                auto child = edge_[i]->children_[(edge_with_this + c) % 4];
                child->edge_[edge_with_this] = children_[(i + 1 - c) % 4];
            }
        }
    }
}

void PlanetTerrainPatch::combine() {
    planet_->terrain_dirty_ = true;

    // For each child, clear its external edges references to the child.
    for (int i = 0; i < 4; ++i) {
        auto child = children_[i];
        if (child->edge_[i]) {
            // Edge pointer back to us is usually offset by 2: (i + 2) mod 4.
            int shared_edge = child->edge_[i]->sharedEdgeWith(child, (i + 2) % 4);
            child->edge_[i]->edge_[shared_edge] = nullptr;
        }
        if (child->edge_[(i + 3) % 4]) {
            // Edge pointer back to us is usually offset by 2: (i + 3) + 2 mod 4 = (i + 1) mod 4.
            int shared_edge = child->edge_[(i + 3) % 4]->sharedEdgeWith(child, (i + 1) % 4);
            child->edge_[(i + 3) % 4]->edge_[shared_edge] = nullptr;
        }
    }

    // Delete children.
    for (int i = 0; i < 4; ++i) {
        planet_->freePatch(children_[i]);
        children_[i] = nullptr;
    }
}

int PlanetTerrainPatch::sharedEdgeWith(PlanetTerrainPatch* patch, int hint) {
    // In some cases, it's possible for the caller to guess correctly which edge will be == patch.
    // 'hint' allows the caller to specify which index to start checking from, to try and avoid
    // iterating as much as possible.
    for (int i = 0; i < 4; ++i) {
        int index = (i + hint) % 4;
        if (edge_[index] == patch) {
            return index;
        }
    }

    // Should never happen. This means the algorithm is faulty.
    assert(false);
    return 0;
}

class Sandbox : public App {
public:
    DW_OBJECT(Sandbox);

    SharedPtr<CameraController> camera_controller;
    SharedPtr<Planet> planet_;

    void init(int argc, char** argv) override {
        auto rc = subsystem<ResourceCache>();
        assert(rc);
        rc->addPath("base", "../media/base");
        rc->addPath("sandbox", "../media/sandbox");

        const float radius = 1000.0f;

        // Create a camera.
        auto& camera = subsystem<EntityManager>()
                           ->createEntity(Position{0.0f, 0.0f, radius * 2}, Quat::identity)
                           .addComponent<Camera>(0.1f, 10000.0f, 60.0f, 1280.0f / 800.0f);
        camera_controller = makeShared<CameraController>(context(), 300.0f);
        camera_controller->possess(&camera);

        // Create a planet.
        planet_ = makeShared<Planet>(context(), radius, 40.0f, &camera);
    }

    void update(float dt) override {
        // Calculate distance to planet and adjust acceleration accordingly.
        auto& a = camera_controller->possessed()->transform()->position();
        auto& b = planet_->position();
        float altitude = a.getRelativeTo(b).Length() - planet_->radius();
        camera_controller->setAcceleration(altitude);

        camera_controller->update(dt);
        planet_->update(dt);
    }

    void render() override {
        subsystem<Renderer>()->setViewClear(0, {0.0f, 0.0f, 0.2f, 1.0f});

        // Calculate average FPS.
        float current_fps = 1.0 / engine_->frameTime();
        static const int FPS_HISTORY_COUNT = 100;
        static float fps_history[FPS_HISTORY_COUNT];
        for (int i = 1; i < FPS_HISTORY_COUNT; ++i) {
            fps_history[i - 1] = fps_history[i];
        }
        fps_history[FPS_HISTORY_COUNT - 1] = current_fps;
        float average_fps = 0.0f;
        for (int i = 0; i < FPS_HISTORY_COUNT; ++i) {
            average_fps += fps_history[i] / FPS_HISTORY_COUNT;
        }

        // Update displayed FPS information every 100ms.
        static double accumulated_time = 0.0;
        static float displayed_fps = 60.0f;
        accumulated_time += engine_->frameTime();
        if (accumulated_time > 1.0f / 30.0f) {
            accumulated_time = 0;
            displayed_fps = average_fps;
        }

        // Display FPS information.
        ImGui::SetNextWindowPos({10, 10});
        ImGui::SetNextWindowSize({140, 40});
        if (!ImGui::Begin("FPS", nullptr, {0, 0}, 0.5f,
                          ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                              ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings)) {
            ImGui::End();
            return;
        }
        ImGui::Text("FPS:   %.1f", displayed_fps);
        ImGui::Text("Frame: %.4f ms", 1000.0f / displayed_fps);
        ImGui::End();
    }

    void shutdown() override {
    }

    String gameName() override {
        return "Sandbox";
    }

    String gameVersion() override {
        return "1.0.0";
    }
};

DW_IMPLEMENT_MAIN(Sandbox);
