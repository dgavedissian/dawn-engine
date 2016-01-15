/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "MaterialGenerator.h"

NAMESPACE_BEGIN

// Class for generating materials for objects to render themselves to the GBuffer
class DW_API GBufferMaterialGenerator : public MaterialGenerator
{
public:
    GBufferMaterialGenerator();

    // The relevant options for objects that are rendered to the GBuffer
    enum GBufferPermutations
    {
        // (Regular) Textures
        GBP_NO_TEXTURES = 0x00000000,
        GBP_ONE_TEXTURE = 0x00000001,
        GBP_TWO_TEXTURES = 0x00000002,
        GBP_THREE_TEXTURES = 0x00000003,
        GBP_TEXTURE_MASK = 0x0000000F,

        // Material properties
        GBP_HAS_DIFFUSE_COLOUR = 0x00000010,

        // The number of texture coordinate sets
        GBP_NO_TEXCOORDS = 0x00000000,
        GBP_ONE_TEXCOORD = 0x00000100,
        GBP_TWO_TEXCOORDS = 0x00000200,
        GBP_TEXCOORD_MASK = 0x00000700,

        // Do we have a normal map
        GBP_NORMAL_MAP = 0x00000800,

        // Are we skinned?
        GBP_SKINNED = 0x00010000
    };

    // The mask of the flags that matter for generating the fragment shader
    static const Ogre::uint32 FS_MASK = 0x0000FFFF;

    // The mask of the flags that matter for generating the vertex shader
    static const Ogre::uint32 VS_MASK = 0x00FFFF00;

    // The mask of the flags that matter for generating the material
    static const Ogre::uint32 MAT_MASK = 0xFF00FFFF;

private:
    String mBaseName;

    virtual Ogre::GpuProgramPtr GenerateVertexShader(MaterialGenerator::Perm permutation) override;
    virtual Ogre::GpuProgramPtr GenerateFragmentShader(MaterialGenerator::Perm permutation) override;
    virtual Ogre::MaterialPtr GenerateTemplateMaterial(MaterialGenerator::Perm permutation) override;
};

NAMESPACE_END
