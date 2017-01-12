/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "LightMaterialGenerator.h"
#include "DeferredLight.h"

namespace dw {

LightMaterialGenerator::LightMaterialGenerator() {
    mVsMask = 0x00000004;
    mFsMask = 0x0000003F;
    mMatMask = LightMaterialGenerator::MI_DIRECTIONAL | LightMaterialGenerator::MI_SHADOW_CASTER;

    mMaterialBaseName = "DeferredShading/Light/";
    mBaseName = mMaterialBaseName;
}

LightMaterialGenerator::~LightMaterialGenerator() {
}

Ogre::GpuProgramPtr LightMaterialGenerator::GenerateVertexShader(Perm permutation) {
    String programName;
    if (permutation & LightMaterialGenerator::MI_DIRECTIONAL)
        programName = "Post/QuadRay/VS";
    else
        programName = "DeferredShading/Light/VS";

    Ogre::GpuProgramPtr ptr =
        Ogre::HighLevelGpuProgramManager::getSingleton().getByName(programName);
    assert(!ptr.isNull());
    return ptr;
}

Ogre::GpuProgramPtr LightMaterialGenerator::GenerateFragmentShader(Perm permutation) {
    // Create shader
    if (mMasterSource.empty()) {
        Ogre::DataStreamPtr ptrMasterSource =
            Ogre::ResourceGroupManager::getSingleton().openResource(
                "light-material.fs", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        assert(ptrMasterSource.isNull() == false);
        mMasterSource = ptrMasterSource->getAsString();
    }

    assert(!mMasterSource.empty());

    // Create name
    String name = mBaseName + "Perm" + Ogre::StringConverter::toString(permutation) + "/FS";

    // Create shader object
    Ogre::HighLevelGpuProgramPtr ptrProgram =
        Ogre::HighLevelGpuProgramManager::getSingleton().createProgram(
            name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, "glsl",
            Ogre::GPT_FRAGMENT_PROGRAM);
    ptrProgram->setSource(mMasterSource);

    // set up the preprocessor defines
    // Important to do this before any call to get parameters, i.e. before the program gets
    // loaded
    ptrProgram->setParameter("preprocessor_defines", GetPPDefines(permutation));

    SetupBaseParameters(ptrProgram->getDefaultParameters());

    // Bind samplers
    Ogre::GpuProgramParametersSharedPtr params = ptrProgram->getDefaultParameters();
    int numSamplers = 0;
    params->setNamedConstant("gb0", (int)numSamplers++);
    params->setNamedConstant("gb1", (int)numSamplers++);

    if (permutation & LightMaterialGenerator::MI_SHADOW_CASTER)
        params->setNamedConstant("ShadowTex", (int)numSamplers++);

    return Ogre::GpuProgramPtr(ptrProgram);
}

Ogre::MaterialPtr LightMaterialGenerator::GenerateTemplateMaterial(Perm permutation) {
    String materialName = mBaseName;

    if (permutation & LightMaterialGenerator::MI_DIRECTIONAL)
        materialName += "Quad";
    else
        materialName += "Geometry";

    if (permutation & LightMaterialGenerator::MI_SHADOW_CASTER)
        materialName += "Shadow";

    return Ogre::MaterialManager::getSingleton().getByName(materialName);
}

String LightMaterialGenerator::GetPPDefines(Perm permutation) {
    String strPPD;

    // Get the type of light
    Ogre::uint lightType = 0;
    if (permutation & LightMaterialGenerator::MI_POINT)
        lightType = 1;
    else if (permutation & LightMaterialGenerator::MI_SPOTLIGHT)
        lightType = 2;
    else if (permutation & LightMaterialGenerator::MI_DIRECTIONAL)
        lightType = 3;
    else
        assert(false && "Permutation must have a light type");

    strPPD += "LIGHT_TYPE=" + Ogre::StringConverter::toString(lightType);

    // Optional parameters
    if (permutation & LightMaterialGenerator::MI_SPECULAR)
        strPPD += ",IS_SPECULAR=1";

    if (permutation & LightMaterialGenerator::MI_ATTENUATED)
        strPPD += ",IS_ATTENUATED=1";

    if (permutation & LightMaterialGenerator::MI_SHADOW_CASTER)
        strPPD += ",IS_SHADOW_CASTER=1";

    return strPPD;
}

void LightMaterialGenerator::SetupBaseParameters(
    const Ogre::GpuProgramParametersSharedPtr& params) {
    assert(params.isNull() == false);

    struct AutoParamPair {
        String name;
        Ogre::GpuProgramParameters::AutoConstantType type;
    };

    // A List of auto params that might be present in the shaders generated
    static const AutoParamPair AUTO_PARAMS[] = {
        {"vpWidth", Ogre::GpuProgramParameters::ACT_VIEWPORT_WIDTH},
        {"vpHeight", Ogre::GpuProgramParameters::ACT_VIEWPORT_HEIGHT},
        {"worldView", Ogre::GpuProgramParameters::ACT_WORLDVIEW_MATRIX},
        {"invProj", Ogre::GpuProgramParameters::ACT_INVERSE_PROJECTION_MATRIX},
        {"invView", Ogre::GpuProgramParameters::ACT_INVERSE_VIEW_MATRIX},
        {"flip", Ogre::GpuProgramParameters::ACT_RENDER_TARGET_FLIPPING},
        {"lightDiffuseColor", Ogre::GpuProgramParameters::ACT_LIGHT_DIFFUSE_COLOUR},
        {"lightSpecularColor", Ogre::GpuProgramParameters::ACT_LIGHT_SPECULAR_COLOUR},
        {"lightFalloff", Ogre::GpuProgramParameters::ACT_LIGHT_ATTENUATION},
        {"lightPos", Ogre::GpuProgramParameters::ACT_LIGHT_POSITION_VIEW_SPACE},
        {"lightDir", Ogre::GpuProgramParameters::ACT_LIGHT_DIRECTION_VIEW_SPACE},
        {"spotParams", Ogre::GpuProgramParameters::ACT_SPOTLIGHT_PARAMS},
        {"farClipDistance", Ogre::GpuProgramParameters::ACT_FAR_CLIP_DISTANCE},
        {"shadowViewProjMat", Ogre::GpuProgramParameters::ACT_TEXTURE_VIEWPROJ_MATRIX}};
    int numParams = sizeof(AUTO_PARAMS) / sizeof(AutoParamPair);
    for (int i = 0; i < numParams; i++) {
        if (params->_findNamedConstantDefinition(AUTO_PARAMS[i].name))
            params->setNamedAutoConstant(AUTO_PARAMS[i].name, AUTO_PARAMS[i].type);
    }
}
}
