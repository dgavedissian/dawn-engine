/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "GeometryUtils.h"

namespace dw {

Ogre::MeshPtr createSphere(const String& name, float radius, int rings, int segments, bool normals,
                           bool texCoords) {
    Ogre::MeshPtr sphere = Ogre::MeshManager::getSingleton().createManual(
        name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Ogre::SubMesh* sphereVertex = sphere->createSubMesh();
    sphere->sharedVertexData = new Ogre::VertexData();
    createSphere(sphere->sharedVertexData, sphereVertex->indexData, radius, rings, segments,
                 normals, texCoords);

    // Set up mesh
    sphereVertex->useSharedVertices = true;
    sphere->_setBounds(Ogre::AxisAlignedBox(Ogre::Vector3(-radius, -radius, -radius),
                                            Ogre::Vector3(radius, radius, radius)),
                       false);
    sphere->_setBoundingSphereRadius(radius);
    sphere->load();
    return sphere;
}

void createSphere(Ogre::VertexData* vertexData, Ogre::IndexData* indexData, float radius, int rings,
                  int segments, bool normals, bool texCoords) {
    assert(vertexData && indexData);

    // Define the vertex format
    Ogre::VertexDeclaration* vertexDecl = vertexData->vertexDeclaration;
    size_t currOffset = 0;

    // Position
    vertexDecl->addElement(0, currOffset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
    currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

    // Normals
    if (normals) {
        vertexDecl->addElement(0, currOffset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
        currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
    }

    // Texture coordinates
    if (texCoords) {
        vertexDecl->addElement(0, currOffset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 0);
    }

    // Allocate the vertex buffer
    vertexData->vertexCount = (rings + 1) * (segments + 1);
    Ogre::HardwareVertexBufferSharedPtr vBuf =
        Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
            vertexDecl->getVertexSize(0), vertexData->vertexCount,
            Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
    Ogre::VertexBufferBinding* binding = vertexData->vertexBufferBinding;
    binding->setBinding(0, vBuf);
    float* vertex = static_cast<float*>(vBuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));

    // Allocate index buffer
    indexData->indexCount = 6 * rings * (segments + 1);
    indexData->indexBuffer = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
        Ogre::HardwareIndexBuffer::IT_16BIT, indexData->indexCount,
        Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
    Ogre::HardwareIndexBufferSharedPtr iBuf = indexData->indexBuffer;
    unsigned short* indices =
        static_cast<unsigned short*>(iBuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));

    float deltaRingAngle = math::pi / rings;
    float deltaSegAngle = 2.0f * math::pi / segments;
    unsigned short wVerticeIndex = 0;

    // Generate the group of rings for the sphere
    for (int ring = 0; ring <= rings; ring++) {
        float r0 = radius * sinf(ring * deltaRingAngle);
        float y0 = radius * cosf(ring * deltaRingAngle);

        // Generate the group of segments for the current ring
        for (int seg = 0; seg <= segments; seg++) {
            float x0 = r0 * sinf(seg * deltaSegAngle);
            float z0 = r0 * cosf(seg * deltaSegAngle);

            // Add one vertex to the strip which makes up the sphere
            *vertex++ = x0;
            *vertex++ = y0;
            *vertex++ = z0;

            if (normals) {
                Ogre::Vector3 vNormal = Ogre::Vector3(x0, y0, z0).normalisedCopy();
                *vertex++ = vNormal.x;
                *vertex++ = vNormal.y;
                *vertex++ = vNormal.z;
            }

            if (texCoords) {
                *vertex++ = (float)seg / (float)segments;
                *vertex++ = (float)ring / (float)rings;
            }

            if (ring != rings) {
                // each vertex (except the last) has six indices pointing to it
                *indices++ = wVerticeIndex + segments + 1;
                *indices++ = wVerticeIndex;
                *indices++ = wVerticeIndex + segments;
                *indices++ = wVerticeIndex + segments + 1;
                *indices++ = wVerticeIndex + 1;
                *indices++ = wVerticeIndex;
                wVerticeIndex++;
            }
        }
    }

    // Unlock
    vBuf->unlock();
    iBuf->unlock();
}

Ogre::MeshPtr createCone(const String& strName, float radius, float height, int verticesInBase) {
    Ogre::MeshPtr cone = Ogre::MeshManager::getSingleton().createManual(
        strName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Ogre::SubMesh* coneVertex = cone->createSubMesh();
    cone->sharedVertexData = new Ogre::VertexData();

    createCone(cone->sharedVertexData, coneVertex->indexData, radius, height, verticesInBase);

    // Set up the cone mesh
    coneVertex->useSharedVertices = true;
    cone->_setBounds(Ogre::AxisAlignedBox(Ogre::Vector3(-radius, 0, -radius),
                                          Ogre::Vector3(radius, height, radius)),
                     false);
    cone->_setBoundingSphereRadius(math::Sqrt(height * height + radius * radius));
    cone->load();
    return cone;
}

void createCone(Ogre::VertexData* vertexData, Ogre::IndexData* indexData, float radius,
                float height, int verticesInBase) {
    assert(vertexData && indexData);

    // Define the vertex format
    Ogre::VertexDeclaration* vertexDecl = vertexData->vertexDeclaration;
    vertexDecl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);

    // Allocate the vertex buffer
    vertexData->vertexCount = verticesInBase + 1;
    Ogre::HardwareVertexBufferSharedPtr vBuf =
        Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
            vertexDecl->getVertexSize(0), vertexData->vertexCount,
            Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
    Ogre::VertexBufferBinding* binding = vertexData->vertexBufferBinding;
    binding->setBinding(0, vBuf);
    float* vertex = static_cast<float*>(vBuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));

    // Allocate index buffer - cone and base
    indexData->indexCount = (3 * verticesInBase) + (3 * (verticesInBase - 2));
    indexData->indexBuffer = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
        Ogre::HardwareIndexBuffer::IT_16BIT, indexData->indexCount,
        Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
    Ogre::HardwareIndexBufferSharedPtr iBuf = indexData->indexBuffer;
    unsigned short* indices =
        static_cast<unsigned short*>(iBuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));

    // Positions: cone head and base
    for (int i = 0; i < 3; i++) {
        *vertex++ = 0.0f;
    }

    // Base:
    float deltaBaseAngle = (2.0f * math::pi) / verticesInBase;
    for (int i = 0; i < verticesInBase; i++) {
        float angle = i * deltaBaseAngle;
        *vertex++ = radius * cosf(angle);
        *vertex++ = height;
        *vertex++ = radius * sinf(angle);
    }

    // Indices:
    // Cone head to vertices
    for (int i = 0; i < verticesInBase; i++) {
        *indices++ = 0;
        *indices++ = (i % verticesInBase) + 1;
        *indices++ = ((i + 1) % verticesInBase) + 1;
    }

    // Cone base
    for (int i = 0; i < verticesInBase - 2; i++) {
        *indices++ = 1;
        *indices++ = i + 3;
        *indices++ = i + 2;
    }

    // Unlock
    vBuf->unlock();
    iBuf->unlock();
}

void createQuad(Ogre::VertexData* vertexData) {
    assert(vertexData);

    vertexData->vertexCount = 4;
    vertexData->vertexStart = 0;

    Ogre::VertexDeclaration* vertexDecl = vertexData->vertexDeclaration;
    Ogre::VertexBufferBinding* bind = vertexData->vertexBufferBinding;

    vertexDecl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);

    Ogre::HardwareVertexBufferSharedPtr vbuf =
        Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
            vertexDecl->getVertexSize(0), vertexData->vertexCount,
            Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

    // Bind buffer
    bind->setBinding(0, vbuf);

    // Upload data
    float data[] = {
        -1.0f, 1.0f,  -1.0f,  // corner 1
        -1.0f, -1.0f, -1.0f,  // corner 2
        1.0f,  1.0f,  -1.0f,  // corner 3
        1.0f,  -1.0f, -1.0f   // corner 4
    };

    vbuf->writeData(0, sizeof(data), data, true);
}
}  // namespace dw
