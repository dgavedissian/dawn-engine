/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Common.h"
#include "OSXGetNSView.h"

#import <AppKit/NSWindow.h>

namespace dw {

uintptr_t getWindowContentViewHandle(SDL_SysWMinfo &info)
{
    NSWindow *window = info.info.cocoa.window;
    NSView *view = [window contentView];
    return (uintptr_t)view;
}

}

