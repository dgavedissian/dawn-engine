/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "core/Collections.h"
#include "core/math/Defs.h"
#include "renderer/Program.h"
#include "renderer/Texture.h"

namespace dw {
class DW_API Material : public Resource {
public:
    DW_OBJECT(Material);

    Material(Context* ctx);
    Material(Context* ctx, SharedPtr<Program> program);
    ~Material() override;

    Result<None> beginLoad(const String& asset_name, InputStream& src) override;

    void setStateEnable(rhi::RenderState state);
    void setStateDisable(rhi::RenderState state);
    void setCullFrontFace(rhi::CullFrontFace front_face);
    void setPolygonMode(rhi::PolygonMode polygon_mode);
    void setBlendEquation(rhi::BlendEquation equation, rhi::BlendFunc src, rhi::BlendFunc dest);
    void setBlendEquation(rhi::BlendEquation equation_rgb, rhi::BlendFunc src_rgb,
                          rhi::BlendFunc dest_rgb, rhi::BlendEquation equation_a,
                          rhi::BlendFunc src_a, rhi::BlendFunc dest_a);
    void setColourWrite(bool colour_write_enabled);
    void setDepthWrite(bool depth_write_enabled);

    void setMask(u32 mask);

    void setTexture(SharedPtr<Texture> texture, uint unit = 0);

    template <typename T> void setUniform(const String& name, const T& value) {
        uniforms_[name] = value;
    }

    void applyRendererState(const Mat4& model_matrix, const Mat4& view_projection_matrix);

    Program* program();

    u32 mask() const;

private:
    SharedPtr<Program> program_;

    HashSet<rhi::RenderState> states_to_enable_;
    HashSet<rhi::RenderState> states_to_disable_;

    rhi::CullFrontFace cull_front_face_;
    rhi::PolygonMode polygon_mode_;
    rhi::BlendEquation blend_equation_rgb_;
    rhi::BlendFunc blend_src_rgb_;
    rhi::BlendFunc blend_dest_rgb_;
    rhi::BlendEquation blend_equation_a_;
    rhi::BlendFunc blend_src_a_;
    rhi::BlendFunc blend_dest_a_;
    bool colour_write_;  // TODO: make component-wise
    bool depth_write_;

    u32 mask_;

    Array<SharedPtr<Texture>, 8> texture_units_;
    HashMap<String, rhi::UniformData> uniforms_;
};
}  // namespace dw
