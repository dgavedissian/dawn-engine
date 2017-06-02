/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "StarSystem.h"

namespace dw {

class DW_API Galaxy {
public:
    Galaxy();
    ~Galaxy();

    /// Create a new star system
    StarSystem* createStarSystem();

    /// Load a star system from a config file
    /// @param config Config filename
    StarSystem* loadStarSystem(const String& config);
};
}  // namespace dw
