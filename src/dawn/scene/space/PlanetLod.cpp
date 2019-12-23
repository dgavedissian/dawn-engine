/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Core.h"
#include "PlanetLod.h"

namespace dw {
// Careful to value-initialize children_ and edge_ in the initialiser list by giving them an
// empty initializer ({}).
PlanetTerrainPatch::PlanetTerrainPatch(PlanetLod* planet, PlanetTerrainPatch* parent,
                                       const Array<Vec3, 4>& corners, int level)
    : planet_{planet}, parent_{parent}, children_{}, edge_{}, corners_(corners), level_{level} {
    // Compute centre position.
    centre_ = Vec3::zero;
    for (auto& c : corners_) {
        centre_ += c * 0.25f;
    }
    centre_ = planet_->calculateHeight(centre_);
    (void)parent_;
}

void PlanetTerrainPatch::setupAdjacentPatches(const Array<PlanetTerrainPatch*, 4>& adjacent) {
    edge_ = adjacent;
}

bool PlanetTerrainPatch::hasChildren() const {
    return children_[0] != nullptr;
}

void PlanetTerrainPatch::updatePatch(const Vec3& offset) {
    float threshold = planet_->patch_split_distance_ / math::Pow(2.0f, static_cast<float>(level_));
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
}  // namespace dw
