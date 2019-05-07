/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#pragma once

#include "resource/Resource.h"

namespace dw {
    class DW_API TextResource : public Resource {
    public:
    DW_OBJECT(TextResource);

    TextResource(Context* ctx);

    Result<void> beginLoad(const String& asset_name, InputStream& src) override;
    Result<void> save(OutputStream& dest) override;

    String data();

    private:
        String data_;
};
}  // namespace dw