/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "core/Collections.h"
#include "renderer/Material.h"
#include "renderer/CustomRenderable.h"
#include "renderer/FrameBuffer.h"

#include <dawn-gfx/Renderer.h>

namespace dw {
class DW_API SceneGraph;

struct DW_API RenderPipelineDesc {
    struct DW_API Texture {
        gfx::TextureFormat format;
        Vec2 ratio = {1.0f, 1.0f};
    };

    struct DW_API ClearStep {
        Colour colour = Colour{};
    };
    struct DW_API RenderQueueStep {
        u32 mask = 0x1;
    };
    struct DW_API RenderQuadStep {
        String material_name = "";
    };

    using Step = Variant<ClearStep, RenderQueueStep, RenderQuadStep>;

    struct DW_API Node {
        HashMap<String, gfx::TextureFormat> inputs = {};
        Vector<Pair<String, gfx::TextureFormat>> outputs = {};
        Vector<Step> steps = {};
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
    explicit RenderPipeline(Context* ctx);
    ~RenderPipeline() override = default;

    static Result<SharedPtr<RenderPipeline>, String> createFromDesc(Context* ctx,
                                                                    const RenderPipelineDesc& desc);

    // Resource.
    Result<void> beginLoad(const String& asset_name, InputStream& src) override;

    void render(float dt, float interpolation, SceneGraph* scene_graph, u32 camera_id);

    SharedPtr<Texture> texture(const String& name);

private:
    class PStep {
    public:
        virtual ~PStep() = default;
        virtual void execute(Logger& log, gfx::Renderer* r, float dt, float interpolation,
                             SceneGraph* scene_graph, u32 camera_id, uint view) = 0;
    };

    class PClearStep : public PStep {
    public:
        PClearStep(Colour colour);

        void execute(Logger& log, gfx::Renderer* r, float dt, float interpolation,
                     SceneGraph* scene_graph, u32 camera_id, uint view) override;

        Colour colour_;
    };

    class PRenderQueueStep : public PStep {
    public:
        PRenderQueueStep(u32 mask);

        void execute(Logger& log, gfx::Renderer* r, float dt, float interpolation,
                     SceneGraph* scene_graph, u32 camera_id, uint view) override;

        u32 mask_;
    };

    class PRenderQuadStep : public PStep {
    public:
        PRenderQuadStep(SharedPtr<VertexBuffer> fullscreen_quad, SharedPtr<Material> material,
                        const HashMap<String, uint>& input_samplers);

        void execute(Logger& log, gfx::Renderer* r, float dt, float interpolation,
                     SceneGraph* scene_graph, u32 camera_id, uint view) override;

        SharedPtr<VertexBuffer> fullscreen_quad_;
        SharedPtr<Material> material_;
        const HashMap<String, uint>& input_samplers_;
    };

    class PNode {
    public:
        PNode();
        ~PNode() = default;

        void prepareForRendering(gfx::Renderer* r, uint view);

        Vector<UniquePtr<PStep>> steps_;

        HashMap<String, SharedPtr<Texture>> input_textures_;
        Vector<SharedPtr<Texture>> output_textures_;
        HashMap<String, uint> input_samplers_;
        UniquePtr<FrameBuffer> output_frame_buffer_;
    };

    HashMap<String, SharedPtr<Texture>> textures_;
    Vector<UniquePtr<PNode>> nodes_;
    SharedPtr<VertexBuffer> fullscreen_quad_;
};
}  // namespace dw
