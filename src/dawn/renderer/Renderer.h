/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

namespace dw {

class Renderer : public Object {
public:
    DW_OBJECT(Renderer);

    Renderer(Context* context);
    ~Renderer();

    void frame();

private:
    uint mWidth, mHeight;
};
}
