/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "GBufferMaterialGenerator.h"

namespace dw {

// Class for handling materials who did not specify techniques for rendering themselves into the
// GBuffer. This class allows deferred shading to be used, without having to specify new techniques
// for all the objects in the scene
class DW_API GBufferSchemeHandler : public Ogre::MaterialManager::Listener {
public:
    virtual Ogre::Technique* handleSchemeNotFound(unsigned short schemeIndex,
                                                  const Ogre::String& schemeName,
                                                  Ogre::Material* originalMaterial,
                                                  unsigned short lodIndex,
                                                  const Ogre::Renderable* rend) override;

protected:
    GBufferMaterialGenerator mMaterialGenerator;

    // The string that will be checked in textures to determine whether they are normal maps
    static const Ogre::String NORMAL_MAP_PATTERN;

    // A structure for containing the properties of a material, relevant to GBuffer rendering
    struct PassProperties {
        PassProperties()
            : isDeferred(true), normalMap(nullptr), isSkinned(false), hasDiffuseColour(false) {
        }

        bool isDeferred;
        Vector<Ogre::TextureUnitState*> regularTextures;
        Ogre::TextureUnitState* normalMap;
        bool isSkinned;
        bool hasDiffuseColour;
    };

    // Inspect a technique and return its relevant properties
    PassProperties InspectPass(Ogre::Pass* pass, unsigned short lodIndex,
                               const Ogre::Renderable* rend);

    // Get the permutation of material flags that fit a certain property sheet
    MaterialGenerator::Perm GetPermutation(const PassProperties& props);

    // Fill a pass with the specific data from the pass it is based on
    void FillPass(Ogre::Pass* gBufferPass, Ogre::Pass* originalPass, const PassProperties& props);

    // Check if a texture is a normal map, and fill property sheet accordingly
    bool CheckNormalMap(Ogre::TextureUnitState* tus, PassProperties& props);
};

// Class for skipping materials which do not have the scheme defined
class DW_API NoGBufferSchemeHandler : public Ogre::MaterialManager::Listener {
public:
    virtual Ogre::Technique* handleSchemeNotFound(unsigned short schemeIndex,
                                                  const Ogre::String& schemeName,
                                                  Ogre::Material* originalMaterial,
                                                  unsigned short lodIndex,
                                                  const Ogre::Renderable* rend) override {
        // Creating a technique so the handler only gets called once per material
        Ogre::Technique* emptyTech = originalMaterial->createTechnique();
        emptyTech->removeAllPasses();
        emptyTech->setSchemeName(schemeName);
        return emptyTech;
    }
};
}  // namespace dw
