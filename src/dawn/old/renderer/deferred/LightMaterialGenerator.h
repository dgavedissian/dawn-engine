/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "MaterialGenerator.h"

namespace dw {

class DW_API LightMaterialGenerator : public MaterialGenerator {
public:
    // Permutation of light materials
    enum MaterialID {
        MI_POINT = 0x01,        // Is a point light
        MI_SPOTLIGHT = 0x02,    // Is a spot light
        MI_DIRECTIONAL = 0x04,  // Is a directional light

        MI_ATTENUATED = 0x08,    // Rendered attenuated
        MI_SPECULAR = 0x10,      // Specular component is calculated
        MI_SHADOW_CASTER = 0x20  // Will cast shadows
    };

    LightMaterialGenerator();
    virtual ~LightMaterialGenerator();

private:
    String mBaseName;
    String mMasterSource;

    virtual Ogre::GpuProgramPtr GenerateVertexShader(Perm permutation) override;
    virtual Ogre::GpuProgramPtr GenerateFragmentShader(Perm permutation) override;
    virtual Ogre::MaterialPtr GenerateTemplateMaterial(Perm permutation) override;

    String GetPPDefines(Perm permutation);
    void SetupBaseParameters(const Ogre::GpuProgramParametersSharedPtr& params);
};
}
