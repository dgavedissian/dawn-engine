/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#define DW_IMPLEMENT_MAIN(AppClass) \
    namespace dw { \
    int runApp(int argc, char** argv) { \
        return dw::runEngine(new AppClass, argc, argv); \
    }

namespace dw {

class App : public Object
{
public:
    App() : Object(nullptr) {}
    virtual ~App() {}

    void _setContext(Context* context) { mContext = context; }

    virtual void init(int argc, char** argv) = 0;
    virtual void update(float dt) = 0;
    virtual void shutdown() = 0;

    virtual String getGameName() = 0;
    virtual String getGameVersion() = 0;
};

int runApp(int argc, char** argv);

}