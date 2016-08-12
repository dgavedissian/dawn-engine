/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "DawnEngine.h"
#include "io/File.h"

class Sandbox : public dw::App {
public:
    DW_OBJECT(Sandbox);

    virtual void init(int argc, char** argv) override {
        dw::File file(mContext, "test.txt", dw::FileMode::Read);
        dw::stream::read<dw::i32>(file);
    }

    virtual void update(float dt) override {
    }

    virtual void shutdown() override {
    }

    virtual dw::String getGameName() override {
        return "Sandbox";
    }

    virtual dw::String getGameVersion() override {
        return "1.0.0";
    }
};

DW_IMPLEMENT_MAIN(Sandbox);