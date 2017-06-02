/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "io/InputStream.h"
#include "resource/Resource.h"

namespace dw {

Resource::Resource(Context* context) : Object(context), mLoaded(false) {
}

bool Resource::load(InputStream& src) {
    if (!beginLoad(src)) {
        return false;
    }
    endLoad();
    mLoaded = true;
    return true;
}

void Resource::endLoad() {
}

void Resource::save(OutputStream&) {
    // Just wanted to say this is kinda nice as typeName() is virtual so whichever resource type
    // doesn't implement saving will write "Save not supported for UnsavableResource" to the log.
    log().error("Save not supported for %s", typeName());
}

bool Resource::hasLoaded() const {
    return mLoaded;
}
}  // namespace dw
