/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "resource/Resource.h"
#include "renderer/Renderer.h"

namespace dw {
class DW_API Shader : public Resource {
public:
    DW_OBJECT(Shader);

    Shader(Context* context, ShaderStage type);
    ~Shader();

    bool beginLoad(InputStream& src) override;
    void endLoad() override;

    ShaderHandle internalHandle() const;

private:
    ShaderStage type_;
    ShaderHandle handle_;
};

class DW_API VertexShader : public Shader {
public:
    DW_OBJECT(VertexShader);

    VertexShader(Context* context) : Shader{context, ShaderStage::Vertex} {
    }
    ~VertexShader() = default;
};

class DW_API GeometryShader : public Shader {
public:
    DW_OBJECT(VertexShader);

    GeometryShader(Context* context) : Shader{context, ShaderStage::Geometry} {
    }
    ~GeometryShader() = default;
};

class DW_API FragmentShader : public Shader {
public:
    DW_OBJECT(VertexShader);

    FragmentShader(Context* context) : Shader{context, ShaderStage::Fragment} {
    }
    ~FragmentShader() = default;
};
}  // namespace dw
