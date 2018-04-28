/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "core/io/InputStream.h"
#include "renderer/Shader.h"
#include "GLSL.h"

namespace {
class GlslangInitialiser {
public:
    GlslangInitialiser() {
        glslang::InitializeProcess();
    }
    ~GlslangInitialiser() {
        glslang::FinalizeProcess();
    }
};
GlslangInitialiser g_glslang_initialiser;
}  // namespace

namespace {
void initResourcesGLSL(TBuiltInResource& resources) {
    resources.maxLights = 32;
    resources.maxClipPlanes = 6;
    resources.maxTextureUnits = 32;
    resources.maxTextureCoords = 32;
    resources.maxVertexAttribs = 64;
    resources.maxVertexUniformComponents = 4096;
    resources.maxVaryingFloats = 64;
    resources.maxVertexTextureImageUnits = 32;
    resources.maxCombinedTextureImageUnits = 80;
    resources.maxTextureImageUnits = 32;
    resources.maxFragmentUniformComponents = 4096;
    resources.maxDrawBuffers = 32;
    resources.maxVertexUniformVectors = 128;
    resources.maxVaryingVectors = 8;
    resources.maxFragmentUniformVectors = 16;
    resources.maxVertexOutputVectors = 16;
    resources.maxFragmentInputVectors = 15;
    resources.minProgramTexelOffset = -8;
    resources.maxProgramTexelOffset = 7;
    resources.maxClipDistances = 8;
    resources.maxComputeWorkGroupCountX = 65535;
    resources.maxComputeWorkGroupCountY = 65535;
    resources.maxComputeWorkGroupCountZ = 65535;
    resources.maxComputeWorkGroupSizeX = 1024;
    resources.maxComputeWorkGroupSizeY = 1024;
    resources.maxComputeWorkGroupSizeZ = 64;
    resources.maxComputeUniformComponents = 1024;
    resources.maxComputeTextureImageUnits = 16;
    resources.maxComputeImageUniforms = 8;
    resources.maxComputeAtomicCounters = 8;
    resources.maxComputeAtomicCounterBuffers = 1;
    resources.maxVaryingComponents = 60;
    resources.maxVertexOutputComponents = 64;
    resources.maxGeometryInputComponents = 64;
    resources.maxGeometryOutputComponents = 128;
    resources.maxFragmentInputComponents = 128;
    resources.maxImageUnits = 8;
    resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
    resources.maxCombinedShaderOutputResources = 8;
    resources.maxImageSamples = 0;
    resources.maxVertexImageUniforms = 0;
    resources.maxTessControlImageUniforms = 0;
    resources.maxTessEvaluationImageUniforms = 0;
    resources.maxGeometryImageUniforms = 0;
    resources.maxFragmentImageUniforms = 8;
    resources.maxCombinedImageUniforms = 8;
    resources.maxGeometryTextureImageUnits = 16;
    resources.maxGeometryOutputVertices = 256;
    resources.maxGeometryTotalOutputComponents = 1024;
    resources.maxGeometryUniformComponents = 1024;
    resources.maxGeometryVaryingComponents = 64;
    resources.maxTessControlInputComponents = 128;
    resources.maxTessControlOutputComponents = 128;
    resources.maxTessControlTextureImageUnits = 16;
    resources.maxTessControlUniformComponents = 1024;
    resources.maxTessControlTotalOutputComponents = 4096;
    resources.maxTessEvaluationInputComponents = 128;
    resources.maxTessEvaluationOutputComponents = 128;
    resources.maxTessEvaluationTextureImageUnits = 16;
    resources.maxTessEvaluationUniformComponents = 1024;
    resources.maxTessPatchComponents = 120;
    resources.maxPatchVertices = 32;
    resources.maxTessGenLevel = 64;
    resources.maxViewports = 16;
    resources.maxVertexAtomicCounters = 0;
    resources.maxTessControlAtomicCounters = 0;
    resources.maxTessEvaluationAtomicCounters = 0;
    resources.maxGeometryAtomicCounters = 0;
    resources.maxFragmentAtomicCounters = 8;
    resources.maxCombinedAtomicCounters = 8;
    resources.maxAtomicCounterBindings = 1;
    resources.maxVertexAtomicCounterBuffers = 0;
    resources.maxTessControlAtomicCounterBuffers = 0;
    resources.maxTessEvaluationAtomicCounterBuffers = 0;
    resources.maxGeometryAtomicCounterBuffers = 0;
    resources.maxFragmentAtomicCounterBuffers = 1;
    resources.maxCombinedAtomicCounterBuffers = 1;
    resources.maxAtomicCounterBufferSize = 16384;
    resources.maxTransformFeedbackBuffers = 4;
    resources.maxTransformFeedbackInterleavedComponents = 64;
    resources.maxCullDistances = 8;
    resources.maxCombinedClipAndCullDistances = 8;
    resources.maxSamples = 4;
    resources.limits.nonInductiveForLoops = 1;
    resources.limits.whileLoops = 1;
    resources.limits.doWhileLoops = 1;
    resources.limits.generalUniformIndexing = 1;
    resources.limits.generalAttributeMatrixVectorIndexing = 1;
    resources.limits.generalVaryingIndexing = 1;
    resources.limits.generalSamplerIndexing = 1;
    resources.limits.generalVariableIndexing = 1;
    resources.limits.generalConstantMatrixVectorIndexing = 1;
}
}  // namespace

namespace dw {
Shader::Shader(Context* context, rhi::ShaderStage type) : Resource{context}, type_{type} {
}

bool Shader::beginLoad(const String&, InputStream& src) {
    u32 src_len = static_cast<u32>(src.size());
    assert(src_len != 0);
    char* src_data = new char[src_len + 1];
    src.readData(src_data, src_len);
    src_data[src_len] = '\0';

    // Parse GLSL code.
    EShLanguage stage = EShLangVertex;
    switch (type_) {
        case rhi::ShaderStage::Vertex:
            stage = EShLangVertex;
            break;
        case rhi::ShaderStage::Geometry:
            stage = EShLangGeometry;
            break;
        case rhi::ShaderStage::Fragment:
            stage = EShLangFragment;
            break;
        default:
            assert(false);
            break;
    }
    glslang::TShader shader{stage};
    const char* shader_strings[1];
    shader_strings[0] = src_data;
    shader.setStrings(shader_strings, 1);
    TBuiltInResource resources{};
    initResourcesGLSL(resources);
    if (!shader.parse(&resources, 330, false, EShMsgDefault)) {
        // Error when compiling.
        log().error("GLSL Compile error: %s", shader.getInfoLog());
        log().error("GLSL Debug log: %s", shader.getInfoDebugLog());
        return false;
    }
    glslang::TProgram program;
    program.addShader(&shader);
    if (!program.link(EShMsgDefault)) {
        // Error when linking.
        log().error("GLSL Compile error: %s", program.getInfoLog());
        log().error("GLSL Debug log: %s", program.getInfoDebugLog());
        return false;
    }

    // Convert to SPIR-V and hand to renderer.
    if (!module<Renderer>()) {
        return false;
    }
    Vector<u32> spirv_out;
    glslang::GlslangToSpv(*program.getIntermediate(stage), spirv_out);
    handle_ = module<Renderer>()->rhi()->createShader(type_, spirv_out.data(),
                                                      spirv_out.size() * sizeof(u32));

    return true;
}

void Shader::endLoad() {
}

rhi::ShaderHandle Shader::internalHandle() const {
    return handle_;
}
}  // namespace dw
