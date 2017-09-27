/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Renderable.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"

namespace dw {
class BillboardSet : public Renderable, public Object {
public:
    DW_OBJECT(BillboardSet);

    BillboardSet(Context* ctx, u32 particle_count, float particle_size);

    void resize(u32 particle_count);

    void setParticlePosition(int particle_id, const Vec3& position);
    void setParticleSize(int particle_id, const Vec2& size);

    void draw(Renderer* renderer, uint view, Transform* camera, const Mat4&,
              const Mat4& view_projection_matrix) override;

private:
    float particle_size_;

    struct ParticleData {
        Vec3 position;
        Vec2 size;
    };
    Vector<ParticleData> particles_;
    struct ParticleVertex {
        Vec3 position;
        Vec2 uv;
    };
    Vector<ParticleVertex> vertex_data_;
    Vector<u32> index_data_;

    SharedPtr<VertexBuffer> vb_;
    SharedPtr<IndexBuffer> ib_;

    void update(Transform* camera_transform);
    void calculateAxes(Transform* camera_transform, const Vec3& position, Vec3& axis_x,
                       Vec3& axis_y);
};
}  // namespace dw
