/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Material.h"
#include "renderer/CustomMeshRenderable.h"
#include "renderer/FrameBuffer.h"
#include "renderer/rhi/Renderer.h"

namespace dw {
class DW_API SceneGraph;

struct DW_API RenderPipelineDesc {
    struct DW_API Texture {
        rhi::TextureFormat format;
        Vec2 ratio = {1.0f, 1.0f};
    };

    struct DW_API ClearStep {
        Colour colour = Colour{};
    };
    struct DW_API RenderQueueStep {
        u32 mask = 0;
    };
    struct DW_API RenderQuadStep {
        String material_name = "";
    };

    struct DW_API Node {
        HashMap<String, rhi::TextureFormat> inputs = {};
        Vector<Pair<String, rhi::TextureFormat>> outputs = {};
        Vector<Variant<ClearStep, RenderQueueStep, RenderQuadStep>> steps = {};
    };

    struct DW_API NodeInstance {
        String node = "";
        // Binding from input to texture.
        HashMap<String, String> input_bindings = {};
        // Binding from output to texture.
        HashMap<String, String> output_bindings = {};
    };

    HashMap<String, Node> nodes = {};
    HashMap<String, Texture> textures = {};
    Vector<NodeInstance> pipeline = {};

    static const char* PipelineOutput;
};

class DW_API RenderPipeline : public Resource {
public:
    RenderPipeline(Context* ctx);
    ~RenderPipeline() override = default;

    static SharedPtr<RenderPipeline> createFromDesc(Context* ctx, const RenderPipelineDesc& desc);

    bool beginLoad(const String& asset_name, InputStream& src) override;
    void endLoad() override;

    void render(float interpolation, SceneGraph* scene_graph, u32 camera_id);

private:
    class PStep {
    public:
        virtual ~PStep() = default;
        virtual void execute(Logger& log, rhi::Renderer* r, float interpolation,
                             SceneGraph* scene_graph, u32 camera_id, uint view) = 0;
    };

    class PClearStep : public PStep {
    public:
        PClearStep(Colour colour);

        void execute(Logger& log, rhi::Renderer* r, float interpolation, SceneGraph* scene_graph,
                     u32 camera_id, uint view) override;

        Colour colour_;
    };

    class PRenderQueueStep : public PStep {
    public:
        PRenderQueueStep(u32 mask);

        void execute(Logger& log, rhi::Renderer* r, float interpolation, SceneGraph* scene_graph,
                     u32 camera_id, uint view) override;

        u32 mask_;
    };

    class PRenderQuadStep : public PStep {
    public:
        PRenderQuadStep(SharedPtr<VertexBuffer> fullscreen_quad, SharedPtr<Material> material,
                        const HashMap<String, uint>& input_samplers);

        void execute(Logger& log, rhi::Renderer* r, float interpolation, SceneGraph* scene_graph,
                     u32 camera_id, uint view) override;

        SharedPtr<VertexBuffer> fullscreen_quad_;
        SharedPtr<Material> material_;
        const HashMap<String, uint>& input_samplers_;
    };

    class PNode {
    public:
        PNode();
        ~PNode() = default;

        void prepareForRendering(rhi::Renderer* r, uint view);

        Vector<UniquePtr<PStep>> steps_;

        HashMap<String, SharedPtr<Texture>> input_textures_;
        Vector<SharedPtr<Texture>> output_textures_;
        HashMap<String, uint> input_samplers_;
        UniquePtr<FrameBuffer> output_frame_buffer_;
    };

    Vector<SharedPtr<Texture>> textures_;
    Vector<UniquePtr<PNode>> nodes_;
    SharedPtr<VertexBuffer> fullscreen_quad_;
};
}  // namespace dw
