/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "io/InputStream.h"
#include "renderer/Mesh.h"

namespace dw {

Mesh::Mesh(Context* context) : Resource(context) {
}

Mesh::~Mesh() {
}

bool Mesh::beginLoad(InputStream& src) {
    getLog().warning << "Mesh loading unimplemented";
    return false;
}

void Mesh::endLoad() {
}
}
