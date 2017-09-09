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

bool Resource::load(const String& asset_name, InputStream& src) {
    if (!beginLoad(asset_name, src)) {
        return false;
    }
    endLoad();
    mLoaded = true;
    return true;
}

void Resource::endLoad() {
}

void Resource::save(OutputStream&) {
    log().error("Save not supported for %s", typeName());
}

bool Resource::hasLoaded() const {
    return mLoaded;
}
}  // namespace dw
