/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#if DW_PLATFORM == DW_WIN32
#define WIN32_LEAN_AND_MIN
#define NOMINMAX
#include <Windows.h>
#define DW_IMPLEMENT_MAIN(AppClass)                                \
    int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {         \
        return dw::runApp(makeUnique<AppClass>(), __argc, __argv); \
    }
#else
#define DW_IMPLEMENT_MAIN(AppClass)                            \
    int main(int argc, char** argv) {                          \
        return dw::runApp(makeUnique<AppClass>(), argc, argv); \
    }
#endif

namespace dw {

class DW_API App : public Object {
public:
    App() : Object{nullptr} {
    }
    virtual ~App() {
    }

    virtual void init(int argc, char** argv) = 0;
    virtual void update(float dt) = 0;
    virtual void shutdown() = 0;

    virtual String getGameName() = 0;
    virtual String getGameVersion() = 0;

    friend DW_API int runApp(UniquePtr<App> app, int argc, char** argv);
};

DW_API int runApp(UniquePtr<App> app, int argc, char** argv);
}
