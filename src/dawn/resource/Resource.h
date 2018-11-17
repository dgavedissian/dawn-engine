/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "core/Result.h"
#include "core/io/InputStream.h"
#include "core/io/OutputStream.h"

namespace dw {
class DW_API Resource : public Object {
public:
    DW_OBJECT(Resource);

    Resource(Context* context);
    virtual ~Resource() = default;

    Result<None> load(const String& asset_name, InputStream& src);
    virtual Result<None> beginLoad(const String& asset_name, InputStream& src) = 0;
    virtual Result<None> endLoad();
    virtual Result<None> save(OutputStream& dest);

    bool hasLoaded() const;

protected:
    bool loaded_;
};
}  // namespace dw
