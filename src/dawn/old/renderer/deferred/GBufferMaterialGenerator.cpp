/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "GBufferMaterialGenerator.h"

namespace dw {

// Use this directive to control whether you are writing projective (regular) or linear depth.
#define WRITE_LINEAR_DEPTH

GBufferMaterialGenerator::GBufferMaterialGenerator() {
    mVsMask = VS_MASK;
    mFsMask = FS_MASK;
    mMatMask = MAT_MASK;
    mMaterialBaseName = "DeferredShading/GBuffer/";
    mBaseName = mMaterialBaseName;
}

Ogre::GpuProgramPtr GBufferMaterialGenerator::GenerateVertexShader(
    MaterialGenerator::Perm permutation) {
    std::stringstream ss;

    // Version information
    ss << "#version 330 core" << std::endl;

    ss << "uniform mat4 worldViewProjMatrix;" << std::endl;
    ss << "uniform mat4 worldViewMatrix;" << std::endl;

    uint32 numTexCoords = (permutation & GBufferMaterialGenerator::GBP_TEXCOORD_MASK) >> 8;
    ss << "in vec4 vertex;" << std::endl;
    ss << "in vec3 normal;" << std::endl;

    if (permutation & GBufferMaterialGenerator::GBP_NORMAL_MAP)
        ss << "in vec3 tangent;" << std::endl;

    for (uint32 i = 0; i < numTexCoords; i++)
        ss << "in vec2 uv" << i << ";" << std::endl;

    // TODO : Skinning inputs
    ss << std::endl;

#ifdef WRITE_LINEAR_DEPTH
    ss << "out vec3 oViewPos;" << std::endl;
#else
    ss << "out float oDepth;" << std::endl;
#endif
    ss << "out vec3 oNormal;" << std::endl;

    if (permutation & GBufferMaterialGenerator::GBP_NORMAL_MAP) {
        ss << "out vec3 oTangent;" << std::endl;
        ss << "out vec3 oBiNormal;" << std::endl;
    }

    for (uint32 i = 0; i < numTexCoords; i++)
        ss << "out vec2 oUv" << i << ";" << std::endl;

    ss << "void main()" << std::endl;

    ss << "{" << std::endl;
    ss << "	gl_Position = worldViewProjMatrix * vertex;" << std::endl;
    ss << "	oNormal = (worldViewMatrix * vec4(normal,0)).xyz;" << std::endl;

    if (permutation & GBufferMaterialGenerator::GBP_NORMAL_MAP) {
        ss << "	oTangent = (worldViewMatrix * vec4(tangent,0)).xyz;" << std::endl;
        ss << "	oBiNormal = cross(oNormal, oTangent);" << std::endl;
    }

#ifdef WRITE_LINEAR_DEPTH
    ss << "	oViewPos = (worldViewMatrix * vertex).xyz;" << std::endl;
#else
    ss << "	oDepth = gl_Position.w;" << std::endl;
#endif

    for (uint32 i = 0; i < numTexCoords; i++)
        ss << "	oUv" << i << " = uv" << i << ';' << std::endl;

    ss << "}" << std::endl;

    String programSource = ss.str();
    String programName = mBaseName + "VP_" + Ogre::StringConverter::toString(permutation);

#if OGRE_DEBUG_MODE
    Ogre::LogManager::getSingleton().getDefaultLog()->logMessage(programName + String("\n") +
                                                                 programSource);
#endif

    // Create shader object
    Ogre::HighLevelGpuProgramPtr ptrProgram =
        Ogre::HighLevelGpuProgramManager::getSingleton().createProgram(
            programName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, "glsl",
            Ogre::GPT_VERTEX_PROGRAM);
    ptrProgram->setSource(programSource);

    const Ogre::GpuProgramParametersSharedPtr& params = ptrProgram->getDefaultParameters();
    params->setNamedAutoConstant("worldViewProjMatrix",
                                 Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
    params->setNamedAutoConstant("worldViewMatrix",
                                 Ogre::GpuProgramParameters::ACT_WORLDVIEW_MATRIX);
    ptrProgram->load();

    return Ogre::GpuProgramPtr(ptrProgram);
}

Ogre::GpuProgramPtr GBufferMaterialGenerator::GenerateFragmentShader(
    MaterialGenerator::Perm permutation) {
    std::stringstream ss;

    // Version information
    ss << "#version 330 core" << std::endl;

    if (permutation & GBufferMaterialGenerator::GBP_NORMAL_MAP)
        ss << "uniform sampler2D sNormalMap;" << std::endl;

    uint32 numTextures = permutation & GBufferMaterialGenerator::GBP_TEXTURE_MASK;

    for (uint32 i = 0; i < numTextures; i++)
        ss << "uniform sampler2D sTex" << i << ";" << std::endl;

    if (numTextures == 0 || permutation & GBufferMaterialGenerator::GBP_HAS_DIFFUSE_COLOUR)
        ss << "uniform vec4 cDiffuseColour;" << std::endl;

#ifdef WRITE_LINEAR_DEPTH
    ss << "uniform float cFarDistance;" << std::endl;
#endif

    ss << "uniform float cSpecularity;" << std::endl;

#ifdef WRITE_LINEAR_DEPTH
    ss << "in vec3 oViewPos;" << std::endl;
#else
    ss << "in float oDepth;" << std::endl;
#endif
    ss << "in vec3 oNormal;" << std::endl;

    if (permutation & GBufferMaterialGenerator::GBP_NORMAL_MAP) {
        ss << "in vec3 oTangent;" << std::endl;
        ss << "in vec3 oBiNormal;" << std::endl;
    }

    uint32 numTexCoords = (permutation & GBufferMaterialGenerator::GBP_TEXCOORD_MASK) >> 8;

    for (uint32 i = 0; i < numTexCoords; i++)
        ss << "in vec2 oUv" << i << ';' << std::endl;

    ss << "layout(location = 0) out vec4 gbuffer0;" << std::endl;
    ss << "layout(location = 1) out vec4 gbuffer1;" << std::endl;

    // Encode Normals
    ss << "vec2 encodeNormal(vec3 n) {" << std::endl;
    ss << "	float scale = 1.7777;" << std::endl;
    ss << "	vec2 enc = n.xy / (n.z + 1.0);" << std::endl;
    ss << "	enc /= scale;" << std::endl;
    ss << "	enc = enc * 0.5 + 0.5;" << std::endl;
    ss << "	return enc;" << std::endl;
    ss << "}" << std::endl;

    // Encode Depth
    ss << "vec2 encodeDepth(float v) {" << std::endl;
    ss << "	float scaledDepth = v * 2048.0;" << std::endl;
    ss << "	return vec2(floor(scaledDepth) / 2048.0, fract(scaledDepth));" << std::endl;
    ss << "}" << std::endl;

    ss << "void main() {" << std::endl;

    if (numTexCoords > 0 && numTextures > 0) {
        ss << "gbuffer0.rgb = texture2D(sTex0, oUv0).rgb;" << std::endl;
        if (permutation & GBufferMaterialGenerator::GBP_HAS_DIFFUSE_COLOUR)
            ss << "gbuffer0.rgb *= cDiffuseColour.rgb;" << std::endl;
    } else {
        ss << "gbuffer0.rgb = cDiffuseColour.rgb;" << std::endl;
    }

    ss << "gbuffer0.a = cSpecularity;" << std::endl;

    if (permutation & GBufferMaterialGenerator::GBP_NORMAL_MAP) {
        ss << "vec3 texNormal = (texture2D(sNormalMap, oUv0).rgb - 0.5) * 2.0;" << std::endl;
        ss << "mat3 normalRotation = mat3(oTangent, oBiNormal, oNormal);" << std::endl;
        ss << "gbuffer1.rg = encodeNormal(normalize(texNormal * "
              "normalRotation));"
           << std::endl;
    } else {
        ss << "gbuffer1.rg = encodeNormal(normalize(oNormal));" << std::endl;
    }

#ifdef WRITE_LINEAR_DEPTH
    ss << "gbuffer1.ba = encodeDepth(length(oViewPos) / cFarDistance); " << std::endl;
#else
    ss << "gbuffer1.ba = encodeDepth(oDepth);" << std::endl;
#endif

    ss << "}" << std::endl;

    String programSource = ss.str();
    String programName = mBaseName + "FP_" + Ogre::StringConverter::toString(permutation);

#if OGRE_DEBUG_MODE
    Ogre::LogManager::getSingleton().getDefaultLog()->logMessage(programSource);
#endif

    // Create shader object
    Ogre::HighLevelGpuProgramPtr ptrProgram =
        Ogre::HighLevelGpuProgramManager::getSingleton().createProgram(
            programName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, "glsl",
            Ogre::GPT_FRAGMENT_PROGRAM);
    ptrProgram->setSource(programSource);

    const Ogre::GpuProgramParametersSharedPtr& params = ptrProgram->getDefaultParameters();
    params->setNamedAutoConstant("cSpecularity", Ogre::GpuProgramParameters::ACT_SURFACE_SHININESS);

    if (numTextures == 0 || permutation & GBufferMaterialGenerator::GBP_HAS_DIFFUSE_COLOUR) {
        params->setNamedAutoConstant("cDiffuseColour",
                                     Ogre::GpuProgramParameters::ACT_SURFACE_DIFFUSE_COLOUR);
    }

    // Bind samplers
    int samplerNum = 0;
    if (permutation & GBufferMaterialGenerator::GBP_NORMAL_MAP)
        params->setNamedConstant("sNormalMap", samplerNum++);

    for (uint32 i = 0; i < numTextures; i++, samplerNum++)
        params->setNamedConstant("sTex" + Ogre::StringConverter::toString(i), samplerNum);

#ifdef WRITE_LINEAR_DEPTH
    // TODO : Should this be the distance to the far corner, not the far clip distance?
    params->setNamedAutoConstant("cFarDistance", Ogre::GpuProgramParameters::ACT_FAR_CLIP_DISTANCE);
#endif

    ptrProgram->load();
    return Ogre::GpuProgramPtr(ptrProgram);
}

Ogre::MaterialPtr GBufferMaterialGenerator::GenerateTemplateMaterial(
    MaterialGenerator::Perm permutation) {
    String matName = mBaseName + "Mat_" + Ogre::StringConverter::toString(permutation);

    Ogre::MaterialPtr matPtr = Ogre::MaterialManager::getSingleton().create(
        matName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Ogre::Pass* pass = matPtr->getTechnique(0)->getPass(0);
    pass->setName(mBaseName + "Pass_" + Ogre::StringConverter::toString(permutation));
    pass->setLightingEnabled(false);

    if (permutation & GBufferMaterialGenerator::GBP_NORMAL_MAP)
        pass->createTextureUnitState();

    uint32 numTextures = permutation & GBufferMaterialGenerator::GBP_TEXTURE_MASK;
    for (uint32 i = 0; i < numTextures; i++)
        pass->createTextureUnitState();

    return matPtr;
}
}
