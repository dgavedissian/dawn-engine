/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#if DW_PLATFORM == DW_WIN32
#define WIN32_LEAN_AND_MIN
#define NOMINMAX
#include <Windows.h>
#define DW_IMPLEMENT_MAIN(AppClass)                        \
    int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) { \
        return dw::runApp(new AppClass, __argc, __argv);   \
    }
#else
#define DW_IMPLEMENT_MAIN(AppClass)                  \
    int main(int argc, char** argv) {                \
        return dw::runApp(new AppClass, argc, argv); \
    }
#endif

namespace dw {

class DW_API App : public Object {
public:
    App() : Object(nullptr) {
    }
    virtual ~App() {
    }

    void _setContext(Context* context) {
        mContext = context;
    }

    virtual void init(int argc, char** argv) = 0;
    virtual void update(float dt) = 0;
    virtual void shutdown() = 0;

    virtual String getGameName() = 0;
    virtual String getGameVersion() = 0;
};

DW_API int runApp(App* app, int argc, char** argv);
}
