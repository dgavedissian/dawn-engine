/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "resource/Resource.h"

namespace dw {

class Mesh : public Resource {
public:
    DW_OBJECT(Mesh);

    Mesh(Context* context);
    ~Mesh();

    bool beginLoad(InputStream& src) override;
    void endLoad() override;

private:
};
}