/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "core/math/Defs.h"
#include "resource/ResourceCache.h"
#include "renderer/Material.h"
#include "renderer/CustomRenderable.h"
#include "renderer/SceneGraph.h"
#include <dawn-gfx/VertexDecl.h>

// This file contains a very hacky prototype implementation of a dynamic terrain generation system.

namespace dw {

class PlanetLod;

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
        static gfx::VertexDecl createDecl() {
            return gfx::VertexDecl{}
                .begin()
                .add(gfx::VertexDecl::Attribute::Position, 3, gfx::VertexDecl::AttributeType::Float)
                .add(gfx::VertexDecl::Attribute::Normal, 3, gfx::VertexDecl::AttributeType::Float)
                .add(gfx::VertexDecl::Attribute::TexCoord0, 2,
                     gfx::VertexDecl::AttributeType::Float)
                .end();
        }
    };

    PlanetTerrainPatch(PlanetLod* planet, PlanetTerrainPatch* parent, const Array<Vec3, 4>& corners,
                       int level);

    void setupAdjacentPatches(const Array<PlanetTerrainPatch*, 4>& adjacent);

    bool hasChildren() const;

    void updatePatch(const Vec3& offset);
    void generateGeometry(Vector<Vertex>& vertex_data, Vector<u32>& index_data);

private:
    PlanetLod* planet_;
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

class PlanetLod : public Object {
public:
    DW_OBJECT(PlanetLod);

    PlanetLod(Context* ctx, SceneGraph* scene_graph, float radius, float terrain_max_height,
           Entity* camera)
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
        auto rc = module<ResourceCache>();

        // Set up material.
        auto material = makeShared<Material>(
            context(),
            makeShared<Program>(context(), *rc->get<VertexShader>("base:space/planet.vs"),
                                *rc->get<FragmentShader>("base:space/planet.fs")));
        material->setTexture(*rc->get<Texture>("base:space/planet.jpg"));
        material->setUniform("light_direction", Vec3{1.0f, 0.0f, 0.0f});
        material->setUniform("surface_sampler", 0);
        material->setPolygonMode(gfx::PolygonMode::Wireframe);

        // Set up renderable.
        setupTerrainRenderable();
        custom_mesh_renderable_->setMaterial(material);

        planet_ = scene_graph->root().newChild(SystemPosition::origin);
        planet_->data.renderable = custom_mesh_renderable_;

        // Kick off terrain update thread.
        terrain_update_thread_ = Thread([this]() {
          while (run_update_thread_.load()) {
              // Calculate offset and update patches.
              t_input_lock_.lock();
              SystemPosition camera_position = t_camera_position_;
              SystemPosition planet_position = t_planet_position_;
              t_input_lock_.unlock();
              updateTerrain(camera_position.getRelativeTo(planet_position));

              // If we detected a change in geometry, regenerate.
              if (terrain_dirty_) {
                  terrain_dirty_ = false;

                  LockGuard<Mutex> terrain_data_lock{t_output_lock_};
                  generateTerrainData(t_output_vertices_, t_output_indices_);
                  t_output_ready_ = true;
              }
          }
        });
    }

    ~PlanetLod() {
        run_update_thread_ = false;
        terrain_update_thread_.join();
    }

    SystemPosition& position() const {
        return planet_->position;
    }

    float radius() const {
        return radius_;
    }

    void update(float dt) {
        // Update camera position data.
        {
            LockGuard<Mutex> camera_position_lock{t_input_lock_};
            t_camera_position_ = camera_->transform()->position;
            t_planet_position_ = planet_->position;
        }

        // If we have any new terrain data ready, upload to GPU.
        if (t_output_ready_) {
            LockGuard<Mutex> terrain_data_lock{t_output_lock_};
            uploadTerrainDataToGpu(std::move(t_output_vertices_), std::move(t_output_indices_));
            t_output_vertices_.clear();
            t_output_indices_.clear();
            t_output_ready_ = false;
        }
    }

private:
    Entity* camera_;

    SystemNode* planet_;
    float radius_;

    // Terrain mesh.
    SharedPtr<CustomRenderable> custom_mesh_renderable_;

    // Bool which controls whether the update task thread is running.
    Atomic<bool> run_update_thread_;
    Thread terrain_update_thread_;

    // Update thread data.
    // INPUTS
    SystemPosition t_camera_position_;
    SystemPosition t_planet_position_;
    Mutex t_input_lock_;
    // OUTPUTS
    Vector<PlanetTerrainPatch::Vertex> t_output_vertices_;
    Vector<u32> t_output_indices_;
    Mutex t_output_lock_;
    // OUTPUT READY
    bool t_output_ready_;

    // Terrain structure data.
    Array<PlanetTerrainPatch*, 6> terrain_patches_;  // Patches: +z, +x, -z, -x, +y, -y
    float patch_split_distance_;
    bool terrain_dirty_;  // only used on update thread.
    fBmNoise noise_;

    PlanetTerrainPatch* allocatePatch(PlanetTerrainPatch* parent, const Array<Vec3, 4>& corners,
                                      int level) {
        return new PlanetTerrainPatch(this, parent, corners, level);
    }

    void freePatch(PlanetTerrainPatch* patch) {
        delete patch;
    }

    Vec3 calculateHeight(const Vec3& position) {
        auto sample_position = position.Normalized() * radius_;
        double height_sample =
            noise_.noise(sample_position.x, sample_position.y, sample_position.z);
        return sample_position * (1.0f + height_sample / radius_);
    }

    Vec3 calculateNormal(const Vec3& position) {
        auto centre_sample_location = position.Normalized() * radius_;
        float offset = 1.0f;  // 1 metre.
        float offset_radians = offset / radius_;

        // Generate 4 samples around point.
        Vec3 samples[4];
        Vec3 result_normal = Vec3::zero;
        for (int i = 0; i < 4; ++i) {
            float angle = math::pi * 0.5f * i;
            Vec3 sample_location =
                Vec3::FromSphericalCoordinates(centre_sample_location.ToSphericalCoordinates() +
                                               Vec3(sin(angle), cos(angle), 0.0f) * offset_radians);
            double height_sample =
                noise_.noise(sample_location.x, sample_location.y, sample_location.z);
            samples[i] = sample_location * (1.0f + height_sample / radius_);
        }

        // Compute normals.
        for (int i = 0; i < 4; ++i) {
            result_normal +=
                (samples[i] - position).Cross(samples[(i + 1) % 4] - position).Normalized();
        }
        return result_normal.Normalized();
    }

    void setupTerrainRenderable() {
        // Setup renderable.
        auto vertex_decl = PlanetTerrainPatch::Vertex::createDecl();
        int default_vertex_count = 36;
        int default_index_count = 20;
        custom_mesh_renderable_ = makeShared<CustomRenderable>(
            context(),
            makeShared<VertexBuffer>(context(),
                                     gfx::Memory(default_vertex_count * vertex_decl.stride()),
                                     default_vertex_count, vertex_decl, gfx::BufferUsage::Dynamic),
            makeShared<IndexBuffer>(context(), gfx::Memory(default_index_count * sizeof(u32)),
                                    gfx::IndexBufferType::U32, gfx::BufferUsage::Dynamic));

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
        uploadTerrainDataToGpu(std::move(vertex_data), std::move(index_data));
    }

    void updateTerrain(const Vec3& offset) {
        for (auto& patch : terrain_patches_) {
            if (patch) {  // TODO: remove once patches are initialised properly.
                patch->updatePatch(offset);
            }
        }
    }

    void generateTerrainData(Vector<PlanetTerrainPatch::Vertex>& vertices, Vector<u32>& indices) {
        for (auto patch : terrain_patches_) {
            if (patch) {  // TODO: remove once patches are initialised properly.
                patch->generateGeometry(vertices, indices);
            }
        }
    }

    void uploadTerrainDataToGpu(Vector<PlanetTerrainPatch::Vertex>&& vertices,
                                Vector<u32>&& indices) {
        // Upload to GPU.
        auto num_vertices = vertices.size();
        custom_mesh_renderable_->vertexBuffer()->update(gfx::Memory{std::move(vertices)},
                                                        num_vertices, 0);
        custom_mesh_renderable_->indexBuffer()->update(gfx::Memory{std::move(indices)}, 0);
    }

    friend class PlanetTerrainPatch;
};
}