/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "io/InputStream.h"
#include "io/OutputStream.h"

namespace dw {

class Resource : public Object {
public:
    DW_OBJECT(Resource);

    Resource(Context* context);
    virtual ~Resource();

    bool load(InputStream& src);
    virtual bool beginLoad(InputStream& src) = 0;
    virtual void endLoad();
    virtual void save(OutputStream& dest);

    bool hasLoaded() const;

protected:
    bool mLoaded;
};
}