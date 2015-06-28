/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

#include "StarSystem.h"

NAMESPACE_BEGIN

class DW_API Galaxy
{
public:
    Galaxy();
    ~Galaxy();

    /// Create a new star system
    StarSystem* CreateStarSystem();

    /// Load a star system from a config file
    /// @param config Config filename
    StarSystem* LoadStarSystem(const string& config);
    
};

NAMESPACE_END
