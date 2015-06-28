/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

#include "SystemBody.h"

NAMESPACE_BEGIN

// A body that only serves as an empty point where other bodies orbit around
class DW_API Barycentre : public SystemBody
{
public:
    Barycentre(RenderSystem* rs);
    virtual ~Barycentre();
};

NAMESPACE_END
