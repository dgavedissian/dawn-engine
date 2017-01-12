/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "DawnEngine.h"
#include "io/File.h"

class Sandbox : public dw::App {
public:
    DW_OBJECT(Sandbox);

    void init(int argc, char** argv) override {
        getSubsystem<dw::FileSystem>()->setWorkingDir("media");

        dw::File file(mContext, "sandbox/test.txt", dw::FileMode::Read);
        getLog().info("File contents: %s", dw::stream::read<dw::u8>(file));
    }

    void update(float dt) override {
    }

    void shutdown() override {
    }

    dw::String getGameName() override {
        return "Sandbox";
    }

    dw::String getGameVersion() override {
        return "1.0.0";
    }
};

DW_IMPLEMENT_MAIN(Sandbox);
