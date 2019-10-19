/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "core/io/InputStream.h"
#include "resource/Resource.h"

namespace dw {

Resource::Resource(Context* context) : Object(context), loaded_(false) {
}

Result<void> Resource::load(const String& asset_name, InputStream& src) {
    auto begin_load_result = beginLoad(asset_name, src);
    if (!begin_load_result) {
        return begin_load_result;
    }
    auto end_load_result = endLoad();
    if (!end_load_result) {
        return end_load_result;
    }
    loaded_ = true;
    return Result<void>();
}

Result<void> Resource::endLoad() {
    return Result<void>();
}

Result<void> Resource::save(OutputStream&) {
    return makeError(str::format("Save not supported for %s", typeName()));
}

bool Resource::hasLoaded() const {
    return loaded_;
}
}  // namespace dw
