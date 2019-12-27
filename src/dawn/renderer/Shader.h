/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "resource/Resource.h"
#include <dawn-gfx/Renderer.h>

namespace dw {
class DW_API Shader : public Resource {
public:
    DW_OBJECT(Shader);

    Shader(Context* context, gfx::ShaderStage type, const Vector<String>& compile_definitions);
    ~Shader() override = default;

    // Resource.
    Result<void> beginLoad(const String& asset_name, InputStream& src) override;

    gfx::ShaderHandle internalHandle() const;

private:
    gfx::ShaderStage type_;
    gfx::ShaderHandle handle_;
    Vector<String> compile_definitions_;
};

class DW_API VertexShader : public Shader {
public:
    DW_OBJECT(VertexShader);

    VertexShader(Context* context, const Vector<String>& compile_definitions = {}) : Shader{context, gfx::ShaderStage::Vertex, compile_definitions} {
    }
    ~VertexShader() = default;
};

class DW_API GeometryShader : public Shader {
public:
    DW_OBJECT(GeometryShader);

    GeometryShader(Context* context, const Vector<String>& compile_definitions = {}) : Shader{context, gfx::ShaderStage::Geometry, compile_definitions} {
    }
    ~GeometryShader() = default;
};

class DW_API FragmentShader : public Shader {
public:
    DW_OBJECT(FragmentShader);

    FragmentShader(Context* context, const Vector<String>& compile_definitions = {}) : Shader{context, gfx::ShaderStage::Fragment, compile_definitions} {
    }
    ~FragmentShader() = default;
};
}  // namespace dw
