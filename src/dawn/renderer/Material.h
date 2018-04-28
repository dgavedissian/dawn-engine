/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "core/Option.h"
#include "core/math/Defs.h"
#include "renderer/Program.h"
#include "renderer/Texture.h"

namespace dw {
class DW_API Material : public Object {
public:
    DW_OBJECT(Material);

    Material(Context* context, SharedPtr<Program> program);
    ~Material();

    void setPolygonMode(PolygonMode polygon_mode);

    void setTexture(SharedPtr<Texture> texture, uint unit = 0);

    template <typename T> void setUniform(const String& name, const T& value) {
        program_->setUniform(name, value);
    }

    void applyRendererState(const Mat4& model_matrix, const Mat4& view_projection_matrix);

    Program* program();

private:
    SharedPtr<Program> program_;

    PolygonMode polygon_mode_;
};
}  // namespace dw
