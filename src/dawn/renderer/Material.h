/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "resource/Resource.h"

namespace dw {

class Material : public Resource {
public:
    DW_OBJECT(Material);

    Material(Context* context);
    ~Material();

    bool beginLoad(InputStream& src) override;
    void endLoad() override;

private:
};
}