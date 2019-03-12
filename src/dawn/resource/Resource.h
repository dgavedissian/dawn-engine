/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#pragma once

#include "core/Collections.h"
#include "core/io/InputStream.h"
#include "core/io/OutputStream.h"

namespace dw {
class DW_API Resource : public Object {
public:
    DW_OBJECT(Resource);

    Resource(Context* context);
    virtual ~Resource() = default;

    Result<void> load(const String& asset_name, InputStream& src);
    virtual Result<void> beginLoad(const String& asset_name, InputStream& src) = 0;
    virtual Result<void> endLoad();
    virtual Result<void> save(OutputStream& dest);

    bool hasLoaded() const;

protected:
    bool loaded_;
};
}  // namespace dw
