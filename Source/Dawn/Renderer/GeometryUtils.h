/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

NAMESPACE_BEGIN

// Create a sphere mesh with a given name, radius, number of rings and number of segments
DW_API Ogre::MeshPtr CreateSphere(const String& name, float radius, int rings, int segments,
                                  bool normals, bool texCoords);

// Fill up a fresh copy of VertexData and IndexData with a sphere's coords given the number of rings
// and the number of segments
DW_API void CreateSphere(Ogre::VertexData* vertexData, Ogre::IndexData* indexData, float radius,
                         int rings, int segments, bool normals, bool texCoords);

// Create a cone mesh with a given name, radius and number of vertices in the base. The created cone
// will have its head at 0, 0, 0, and will 'expand to' positive y
DW_API Ogre::MeshPtr CreateCone(const String& name, float radius, float height, int verticesInBase);

// Fill up a fresh copy of VertexData and IndexData with a cone's coords given the radius and number
// of vertices in the base
DW_API void CreateCone(Ogre::VertexData* vertexData, Ogre::IndexData* indexData, float radius,
                       float height, int verticesInBase);

// Fill up a fresh copy of VertexData with a normalized quad
DW_API void CreateQuad(Ogre::VertexData* vertexData);

NAMESPACE_END
