/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "resource/Resource.h"
#include "renderer/rhi/RHIRenderer.h"

namespace dw {
class DW_API Shader : public Resource {
public:
    DW_OBJECT(Shader);

    Shader(Context* context, rhi::ShaderStage type);
    virtual ~Shader() = default;

    // Resource.
    Result<void> beginLoad(const String& asset_name, InputStream& src) override;

    rhi::ShaderHandle internalHandle() const;

private:
    rhi::ShaderStage type_;
    rhi::ShaderHandle handle_;
};

class DW_API VertexShader : public Shader {
public:
    DW_OBJECT(VertexShader);

    VertexShader(Context* context) : Shader{context, rhi::ShaderStage::Vertex} {
    }
    ~VertexShader() = default;
};

class DW_API GeometryShader : public Shader {
public:
    DW_OBJECT(GeometryShader);

    GeometryShader(Context* context) : Shader{context, rhi::ShaderStage::Geometry} {
    }
    ~GeometryShader() = default;
};

class DW_API FragmentShader : public Shader {
public:
    DW_OBJECT(FragmentShader);

    FragmentShader(Context* context) : Shader{context, rhi::ShaderStage::Fragment} {
    }
    ~FragmentShader() = default;
};
}  // namespace dw
