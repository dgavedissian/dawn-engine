/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

namespace dw {
enum class RenderQueueGroup : uint {
    Background = 0,
    Group1 = 1,
    Group2 = 2,
    Group3 = 3,
    Main = 4,
    Group5 = 5,
    Group6 = 6,
    Group7 = 7
};

constexpr auto kRenderQueueGroupCount = 8;

struct RenderQueueGroupMap {
    const std::vector<uint>& render_queues;

    uint renderQueue(RenderQueueGroup group) const {
        assert(uint(group) < render_queues.size());
        return render_queues[uint(group)];
    }
};
}  // namespace dw
