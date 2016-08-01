/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "MaterialGenerator.h"

namespace dw {

MaterialGenerator::MaterialGenerator() : mVsMask(0), mFsMask(0), mMatMask(0) {
}

MaterialGenerator::~MaterialGenerator() {
}

const Ogre::MaterialPtr& MaterialGenerator::GetMaterial(Perm permutation) {
    // Check if material/shader permutation already was generated
    MaterialMap::iterator i = mMaterials.find(permutation);
    if (i != mMaterials.end()) {
        return i->second;
    } else {
        // Create it
        Ogre::MaterialPtr templ = GetTemplateMaterial(permutation & mMatMask);
        Ogre::GpuProgramPtr vs = GetVertexShader(permutation & mVsMask);
        Ogre::GpuProgramPtr fs = GetFragmentShader(permutation & mFsMask);

        // Create material name
        String name = mMaterialBaseName + Ogre::StringConverter::toString(permutation);

        // Create material from template, and set shaders
        Ogre::MaterialPtr mat = templ->clone(name);
        Ogre::Technique* tech = mat->getTechnique(0);
        Ogre::Pass* pass = tech->getPass(0);
        pass->setFragmentProgram(fs->getName());
        pass->setVertexProgram(vs->getName());

        // And store it
        mMaterials[permutation] = mat;
        return mMaterials[permutation];
    }
}

const Ogre::GpuProgramPtr& MaterialGenerator::GetVertexShader(Perm permutation) {
    ProgramMap::iterator i = mVsMap.find(permutation);
    if (i != mVsMap.end()) {
        return i->second;
    } else {
        // Create it
        mVsMap[permutation] = GenerateVertexShader(permutation);
        return mVsMap[permutation];
    }
}

const Ogre::GpuProgramPtr& MaterialGenerator::GetFragmentShader(Perm permutation) {
    ProgramMap::iterator i = mFsMap.find(permutation);
    if (i != mFsMap.end()) {
        return i->second;
    } else {
        // Create it
        mFsMap[permutation] = GenerateFragmentShader(permutation);
        return mFsMap[permutation];
    }
}

const Ogre::MaterialPtr& MaterialGenerator::GetTemplateMaterial(Perm permutation) {
    MaterialMap::iterator i = mTemplateMatMap.find(permutation);
    if (i != mTemplateMatMap.end()) {
        return i->second;
    } else {
        // Create it
        mTemplateMatMap[permutation] = GenerateTemplateMaterial(permutation);
        return mTemplateMatMap[permutation];
    }
}
}
