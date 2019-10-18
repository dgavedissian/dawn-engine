/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Core.h"
#include "resource/TextResource.h"

namespace dw {
TextResource::TextResource(Context* ctx) : Resource(ctx) {
}

Result<void> TextResource::beginLoad(const String&, InputStream& src) {
    auto result = src.readAll();
    if (!result) {
        return makeError(result.error());
    }
    data_ = String(static_cast<const char*>(static_cast<void*>(result->data())), result->size());
    return {};
}

Result<void> TextResource::save(OutputStream& dest) {
    dest.writeData(data_.data(), data_.size());
}
}  // namespace dw