/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

namespace dw {
struct DW_API RenderPipelineDesc {
    struct DW_API RenderTarget {
        int format;
        Vec2 ratio;
    };

    struct DW_API Node {
        struct DW_API ClearStep {};
        struct DW_API RenderQueueStep {};
        struct DW_API RenderQuadStep {};

        Vector<Variant<ClearStep, RenderQueueStep, RenderQuadStep>> steps;
        Vector<String, int> inputs;
        Vector<String, int> outputs;
    };
    
    struct DW_API NodeInstance {
        String node;
        Vector<String> input_bindings;
        Vector<String> output_bindings;
    }

    HashMap<String, Node> nodes;
    HashMap<String, RenderTarget> render_targets;
    Vector<NodeInstance> pipeline;
};

class DW_API RenderPipelineRenderTarget : public Object {
public:
    RenderPipelineRenderTarget(Context* ctx);
    ~RenderPipelineRenderTarget() = default;

private:
    rhi::RenderTarget render_target_;
};

class DW_API RenderPipelineNode : public Object {
public:
    RenderPipelineNode(Context* ctx);
    ~RenderPipelineNode() = default;

private:
    RenderPipelineNode* next_;
};
}  // namespace dw
