/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
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

    Result<void> beginLoad(const String& asset_name, InputStream& src) override;

    void setStateEnable(gfx::RenderState state);
    void setStateDisable(gfx::RenderState state);
    void setCullFrontFace(gfx::CullFrontFace front_face);
    void setPolygonMode(gfx::PolygonMode polygon_mode);
    void setBlendEquation(gfx::BlendEquation equation, gfx::BlendFunc src, gfx::BlendFunc dest);
    void setBlendEquation(gfx::BlendEquation equation_rgb, gfx::BlendFunc src_rgb,
                          gfx::BlendFunc dest_rgb, gfx::BlendEquation equation_a,
                          gfx::BlendFunc src_a, gfx::BlendFunc dest_a);
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

    HashSet<gfx::RenderState> states_to_enable_;
    HashSet<gfx::RenderState> states_to_disable_;

    gfx::CullFrontFace cull_front_face_;
    gfx::PolygonMode polygon_mode_;
    gfx::BlendEquation blend_equation_rgb_;
    gfx::BlendFunc blend_src_rgb_;
    gfx::BlendFunc blend_dest_rgb_;
    gfx::BlendEquation blend_equation_a_;
    gfx::BlendFunc blend_src_a_;
    gfx::BlendFunc blend_dest_a_;
    bool colour_write_;  // TODO: make component-wise
    bool depth_write_;

    u32 mask_;

    Array<SharedPtr<Texture>, 8> texture_units_;
    HashMap<String, gfx::UniformData> uniforms_;
};
}  // namespace dw
