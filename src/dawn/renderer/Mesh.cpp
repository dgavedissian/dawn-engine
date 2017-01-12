/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
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
    getLog().error("Mesh loading unimplemented");
    return false;
}

void Mesh::endLoad() {
}
}
