/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Geometry.h"
#include "resource/Resource.h"

namespace dw {

class Mesh : public Resource, public Geometry {
public:
    DW_OBJECT(Mesh);

    Mesh(Context* context);
    ~Mesh();

    bool beginLoad(InputStream& src) override;
    void endLoad() override;

private:
};
}
