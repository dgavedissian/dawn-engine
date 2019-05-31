/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Core.h"

using namespace dw;

class Viewer : public App {
public:
    DW_OBJECT(Viewer);

    Viewer() : App("Viewer", DW_VERSION_STR) {
    }

    void init(const CommandLine&) override {
    }

    void shutdown() override {
    }
};

DW_IMPLEMENT_MAIN(Viewer);
