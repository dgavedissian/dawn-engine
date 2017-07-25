/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "io/InputStream.h"
#include "renderer/Mesh.h"

#define ASSIMP_BUILD_BOOST_WORKAROUND
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace dw {
Mesh::Mesh(Context* context) : Resource(context) {
}

Mesh::~Mesh() {
}

bool Mesh::beginLoad(InputStream& is) {
    // Read stream.
    assert(is.size() > 0);
    u64 size = is.size();
    byte* data = new byte[size];
    is.read(data, size);
    assert(is.eof());

    // Run importer.
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFileFromMemory(
        data, size, aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                        aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

    return false;
}

void Mesh::endLoad() {
}
}  // namespace dw
