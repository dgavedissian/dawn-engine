/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "CProjectile.h"
#include "scene/SceneManager.h"
#include "net/CNetData.h"
#include "net/NetInstance.h"

FreeListAllocator::FreeListAllocator() : FreeListAllocator(0) {
}

FreeListAllocator::FreeListAllocator(int size) : size_(size) {
    for (int i = 0; i < size; ++i) {
        free_.emplace_back(i);
    }
}

Option<int> FreeListAllocator::allocate() {
    if (free_.empty()) {
        return {};
    }

    int id = free_.front();
    free_.pop_front();
#ifdef DW_DEBUG
    allocated_.emplace(id);
#endif
    return id;
}

void FreeListAllocator::free(int index) {
#ifdef DW_DEBUG
    assert(allocated_.find(index) != allocated_.end());
#endif
    free_.emplace_back(index);
}

void FreeListAllocator::resize(int new_size) {
    assert(new_size >= size_);
    for (int i = size_; i < new_size; ++i) {
        free_.emplace_back(i);
    }
}

int FreeListAllocator::size() const {
    return size_;
}

SProjectile::SProjectile(Context* ctx, SceneManager* scene_manager, NetInstance* net, Frame* frame,
                         const HashMap<int, ProjectileTypeInfo>& types)
    : EntitySystem(ctx), types_(types), scene_manager_(scene_manager), net_(net) {
    supportsComponents<CProjectile>();

    const auto billboard_count = 200;

    for (auto& type : types_) {
        auto* node = frame->newChild();
        auto renderable = makeShared<BillboardSet>(ctx, billboard_count, type.second.size);
        renderable->material()->setTexture(type.second.texture);
        renderable->setBillboardType(BillboardType::Directional);
        for (int i = 0; i < billboard_count; ++i) {
            renderable->setParticleVisible(i, false);
        }
        node->data.renderable = renderable;
        render_data_[type.first] = {node, renderable.get(), FreeListAllocator{billboard_count}};
    }
}

Entity* SProjectile::createNewProjectile(int type, const Vec3& position, const Vec3& direction,
                                         const Vec3& velocity, const Colour& colour) {
    assert(types_.find(type) != types_.end());

    auto& render_data = render_data_.at(type);

    // Allocate a billboard.
    Option<int> billboard_id = render_data.free_billboards.allocate();
    if (!billboard_id.isPresent()) {
        log().warn("Unable to create billboard. Ran out of IDs. Size: %d",
                   render_data.free_billboards.size());
        return nullptr;
    }

    render_data.billboard_set->setParticleVisible(billboard_id.get(), true);

    auto& entity = scene_manager_->createEntity(Hash("Projectile"));
    entity.addComponent<CProjectile>();
    auto& projectile = *entity.component<CProjectile>();
    projectile.type = type;
    projectile.particle_id = billboard_id.get();
    projectile.position = position;
    projectile.direction = direction;
    projectile.velocity = velocity;
    projectile.colour = colour;

    // Add net data and replicate.
    if (net_) {
        entity.addComponent<CNetData>(net_, RepLayout::build<CProjectile>());
        net_->replicateEntity(entity);
    }

    return &entity;
}

void SProjectile::processEntity(Entity& entity, float dt) {
    auto& data = *entity.component<CProjectile>();
    auto& render_data = render_data_.at(data.type);

    // Perform a raycast between the old and new position.
    Vec3 new_position = data.position + data.velocity * dt;
    // TODO: Raycast.

    // Update position.
    data.position = new_position;

    // Update billboard.
    render_data.billboard_set->setParticleDirection(data.particle_id, data.direction);
    render_data.billboard_set->setParticlePosition(data.particle_id, data.position);
}
