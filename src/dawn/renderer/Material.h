/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "core/Option.h"
#include "math/Defs.h"
#include "renderer/Program.h"
#include "renderer/Texture.h"

namespace dw {
class DW_API Material : public Object {
public:
    DW_OBJECT(Material);

    Material(Context* context, SharedPtr<Program> program);
    ~Material();

    void setTextureUnit(SharedPtr<Texture> texture, uint unit = 0);

    template <typename T> void setUniform(const String& name, const T& value) {
        program_->setUniform(name, value);
    }

private:
    SharedPtr<Program> program_;
};
}  // namespace dw
