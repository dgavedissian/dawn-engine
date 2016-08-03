/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "engine/App.h"
#include "platform/Entry.h"

namespace dw {

int entryPoint(int argc, char** argv) {
    return dw::runApp(argc, argv);
}

}

#if DW_PLATFORM == DW_WIN32
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPCSTR, int) {
    return dw::entryPoint(__argc, __argv);
}
#else
int main(int argc, char** argv) {
    return dw::entryPoint(argc, argv);
}
#endif