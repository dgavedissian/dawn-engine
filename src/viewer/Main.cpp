/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "DawnEngine.h"

using namespace dw;

class Viewer : public App {
public:
    DW_OBJECT(Viewer);

    void init(const CommandLine&) override {
    }

    void update(float) override {
    }

    void shutdown() override {
    }

    String gameName() override {
        return "Viewer";
    }

    String gameVersion() override {
        return DW_VERSION_STR;
    }
};

DW_IMPLEMENT_MAIN(Viewer);
