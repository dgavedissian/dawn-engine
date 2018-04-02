/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "Rect.h"

namespace dw {

Rect::Rect() : begin(), end() {
}

Rect::Rect(int x1, int y1, int x2, int y2) : begin(x1, y1), end(x2, y2) {
}

Rect::Rect(const Vec2i& _begin, const Vec2i& _end) : begin(_begin), end(_end) {
}

Rect& Rect::merge(const Rect& other) {
    begin.x = math::Min(begin.x, other.begin.x);
    begin.y = math::Min(begin.y, other.begin.y);
    end.x = math::Max(end.x, other.end.x);
    end.y = math::Max(end.y, other.end.y);
    return *this;
}
}  // namespace dw
