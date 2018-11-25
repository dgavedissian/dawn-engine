/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "resource/Resource.h"
#include "renderer/Texture.h"
#include "renderer/FrameBuffer.h"
#include "renderer/RenderPipeline.h"
#include "renderer/CustomMeshRenderable.h"
#include "renderer/Renderer.h"
#include "renderer/SceneGraph.h"
#include "resource/ResourceCache.h"

//#define ENABLE_DEBUG_LOGGING

namespace dw {
const char* RenderPipelineDesc::PipelineOutput = "__OUTPUT__";

RenderPipeline::RenderPipeline(Context* ctx) : Resource{ctx} {
}

Result<SharedPtr<RenderPipeline>, String> RenderPipeline::createFromDesc(
    Context* ctx, const RenderPipelineDesc& desc) {
    auto& log = ctx->module<Logger>()->withObjectName("dw::RenderPipeline");

    // Verify that the pipeline nodes fit together.
    for (auto& node_instance : desc.pipeline) {
        auto node_it = desc.nodes.find(node_instance.node);
        if (node_it == desc.nodes.end()) {
            return {str::format("Node '%s' does not exist.", node_instance.node)};
        }

        auto& node = node_it->second;

        // Verify that all inputs and outputs are bound.
        if (node.inputs.size() != node_instance.input_bindings.size()) {
            return {str::format(
                "Mismatching input bindings. Number of input is %d but number of bindings is %d.",
                node.inputs.size(), node_instance.input_bindings.size())};
        }
        if (node.outputs.size() != node_instance.output_bindings.size()) {
            return {str::format(
                "Mismatching output bindings. Number of outputs is %d but number of bindings is "
                "%d.",
                node.outputs.size(), node_instance.output_bindings.size())};
        }

        // Verify that the bindings make sense.
        HashSet<String> inputs_bound;
        HashSet<String> textures_bound_to_inputs;
        for (auto& binding : node_instance.input_bindings) {
            auto input_it = node.inputs.find(binding.first);
            if (input_it == node.inputs.end()) {
                return {str::format("Input '%s' does not exist.", binding.first)};
            }

            if (inputs_bound.count(binding.first) > 0) {
                return {str::format("Input '%s' is already bound.", binding.first)};
            }

            if (textures_bound_to_inputs.count(binding.second) > 0) {
                return {str::format("Texture '%s' is already bound.", binding.second)};
            }

            auto texture_it = desc.textures.find(binding.second);
            if (texture_it == desc.textures.end()) {
                return {str::format("Texture '%s' bound to '%s' doesn't exist.", binding.second,
                                    binding.first)};
            }

            if (input_it->second != texture_it->second.format) {
                return {str::format("Texture format mismatch. Input: %s (%d). Texture: %s (%d)",
                                    input_it->first, static_cast<int>(input_it->second),
                                    texture_it->first,
                                    static_cast<int>(texture_it->second.format))};
            }
        }
        HashSet<String> outputs_bound;
        HashSet<String> textures_bound_to_outputs;
        Option<rhi::TextureFormat> output_format;
        for (auto& binding : node_instance.output_bindings) {
            auto output_it =
                std::find_if(node.outputs.begin(), node.outputs.end(),
                             [&binding](const Pair<String, rhi::TextureFormat>& item) -> bool {
                                 return item.first == binding.first;
                             });
            if (output_it == node.outputs.end()) {
                return {str::format("Output '%s' does not exist.", binding.first)};
            }

            if (outputs_bound.count(binding.first) > 0) {
                return {str::format("Output '%s' is already bound.", binding.first)};
            }

            if (textures_bound_to_outputs.count(binding.second) > 0) {
                return {str::format("Texture '%s' is already bound.", binding.second)};
            }

            // As all outputs are bound to a single render target (called a multiple render target),
            // we should ensure that the formats are identical.
            if (output_format.isPresent()) {
                if (output_it->second != *output_format) {
                    return {str::format(
                        "Texture format mismatch. Invalid MRT. Output %s is %d, but expecting %d.",
                        output_it->first, static_cast<int>(output_it->second),
                        static_cast<int>(*output_format))};
                }
            } else {
                output_format = output_it->second;
            }

            if (binding.second == RenderPipelineDesc::PipelineOutput) {
                if (output_it->second != rhi::TextureFormat::RGBA8) {
                    return {str::format(
                        "Texture format mismatch. Output: %s (%d). Texture: Output (RGBA8)",
                        output_it->first, static_cast<int>(output_it->second))};
                }
            } else {
                auto texture_it = desc.textures.find(binding.second);
                if (texture_it == desc.textures.end()) {
                    return {str::format("Texture '%s' bound to '%s' doesn't exist.", binding.second,
                                        binding.first)};
                }

                if (output_it->second != texture_it->second.format) {
                    return {str::format(
                        "Texture format mismatch. Output: %s (%d). Texture: %s (%d)",
                        output_it->first, static_cast<int>(output_it->second), texture_it->first,
                        static_cast<int>(texture_it->second.format))};
                }
            }
        }

        // Check that steps make sense.
        // TODO: Check that material resource exists.
    }

    auto render_pipeline = makeShared<RenderPipeline>(ctx);

    // Create quad mesh.
    // clang-format off
    float vertices[] = {
        // Position | UV
        -1.0f, -1.0f, 0.0f, 0.0f,
        3.0f,  -1.0f, 2.0f, 0.0f,
        -1.0f,  3.0f, 0.0f, 2.0f
    };
    // clang-format on
    rhi::VertexDecl decl;
    decl.begin()
        .add(rhi::VertexDecl::Attribute::Position, 2, rhi::VertexDecl::AttributeType::Float)
        .add(rhi::VertexDecl::Attribute::TexCoord0, 2, rhi::VertexDecl::AttributeType::Float)
        .end();
    render_pipeline->fullscreen_quad_ =
        makeShared<VertexBuffer>(ctx, Memory(vertices, sizeof(vertices)), 3, decl);

    // Create textures.
    for (auto& texture_desc : desc.textures) {
        render_pipeline->textures_[texture_desc.first] = Texture::createTexture2D(
            ctx, ctx->module<Renderer>()->rhi()->backbufferSize() * texture_desc.second.ratio,
            texture_desc.second.format);
    }

    // Build nodes.
    const auto& textures = render_pipeline->textures_;
    for (auto& node_instance : desc.pipeline) {
        HashMap<String, SharedPtr<Texture>> input_textures;
        Vector<SharedPtr<Texture>> output_textures;
        bool backbuffer_output = false;

        auto& node_desc = desc.nodes.at(node_instance.node);

        // Look up textures bound to inputs and outputs.
        for (auto& input_binding : node_instance.input_bindings) {
            input_textures[input_binding.first] = textures.at(input_binding.second);
        }
        if (node_instance.output_bindings.at(node_desc.outputs[0].first) !=
            RenderPipelineDesc::PipelineOutput) {
            for (auto& output : node_desc.outputs) {
                auto texture_name = node_instance.output_bindings.at(output.first);
                output_textures.emplace_back(textures.at(texture_name));
            }
        } else {
            backbuffer_output = true;
        }

        // Create node.
        UniquePtr<PNode> node = makeUnique<PNode>();
        node->input_textures_ = std::move(input_textures);
        node->output_textures_ = std::move(output_textures);

        // Set up input samplers.
        uint index = 0;
        for (auto& input : node->input_textures_) {
            node->input_samplers_[input.first] = index++;
        }

        // Set up steps.
        for (auto& step_desc : node_desc.steps) {
            UniquePtr<PStep> step;
            if (step_desc.is<RenderPipelineDesc::ClearStep>()) {
                auto clear_step_desc = step_desc.get<RenderPipelineDesc::ClearStep>();
                step = makeUnique<PClearStep>(clear_step_desc.colour);
            }
            if (step_desc.is<RenderPipelineDesc::RenderQueueStep>()) {
                auto render_queue_step_desc = step_desc.get<RenderPipelineDesc::RenderQueueStep>();
                step = makeUnique<PRenderQueueStep>(render_queue_step_desc.mask);
            }
            if (step_desc.is<RenderPipelineDesc::RenderQuadStep>()) {
                auto render_quad_step_desc = step_desc.get<RenderPipelineDesc::RenderQuadStep>();
                auto material = ctx->module<ResourceCache>()->get<Material>(
                    render_quad_step_desc.material_name);
                if (material.hasError()) {
                    return {str::format("Unable to set up material in render quad step. Reason: %s",
                                        material.error())};
                }
                auto material_instance = makeShared<Material>(**material);
                for (auto& sampler : node->input_samplers_) {
                    material_instance->setUniform<int>(sampler.first + "_sampler", sampler.second);
                    material_instance->setTexture(node->input_textures_.at(sampler.first),
                                                  sampler.second);
                }
                step = makeUnique<PRenderQuadStep>(render_pipeline->fullscreen_quad_,
                                                   material_instance, node->input_samplers_);
            }
            node->steps_.push_back(std::move(step));
        }

        // Create output frame buffer.
        node->output_frame_buffer_ =
            backbuffer_output ? nullptr : makeUnique<FrameBuffer>(ctx, node->output_textures_);

        // Add node.
        render_pipeline->nodes_.push_back(std::move(node));
    }

    return render_pipeline;
}

Result<None> RenderPipeline::beginLoad(const String& asset_name, InputStream& src) {
    return {"Render pipeline loading unimplemented."};
}

void RenderPipeline::render(float interpolation, SceneGraph* scene_graph, u32 camera_id) {
    auto rhi = module<Renderer>()->rhi();
    for (uint view = 0; view < nodes_.size(); ++view) {
        nodes_[view]->prepareForRendering(rhi, view);
    }
    for (uint view = 0; view < nodes_.size(); ++view) {
        for (auto& step : nodes_[view]->steps_) {
            step->execute(log().withObjectName("dw::RenderPipeline"), rhi, interpolation,
                          scene_graph, camera_id, view);
        }
    }
}

SharedPtr<Texture> RenderPipeline::texture(const String& name) {
    auto it = textures_.find(name);
    return it != textures_.end() ? it->second : nullptr;
}

RenderPipeline::PClearStep::PClearStep(Colour colour) : colour_(colour) {
}

void RenderPipeline::PClearStep::execute(Logger& log, rhi::RHIRenderer* r, float interpolation,
                                         SceneGraph* scene_graph, u32 camera_id, uint view) {
#ifdef ENABLE_DEBUG_LOGGING
    log.debug("Setting view clear to %s", colour_.rgba().ToString());
#endif
    r->setViewClear(view, colour_);
}

RenderPipeline::PRenderQueueStep::PRenderQueueStep(u32 mask) : mask_(mask) {
}

void RenderPipeline::PRenderQueueStep::execute(Logger& log, rhi::RHIRenderer* r,
                                               float interpolation, SceneGraph* scene_graph,
                                               u32 camera_id, uint view) {
#ifdef ENABLE_DEBUG_LOGGING
    log.debug("Rendering scene from camera %d (mask: 0x%x) to view %d", camera_id, mask_, view);
#endif
    scene_graph->renderSceneFromCamera(interpolation, camera_id, view, mask_);
}

RenderPipeline::PRenderQuadStep::PRenderQuadStep(SharedPtr<VertexBuffer> fullscreen_quad,
                                                 SharedPtr<Material> material,
                                                 const HashMap<String, uint>& input_samplers)
    : fullscreen_quad_(std::move(fullscreen_quad)),
      material_(std::move(material)),
      input_samplers_(input_samplers) {
}

void RenderPipeline::PRenderQuadStep::execute(Logger& log, rhi::RHIRenderer* r, float interpolation,
                                              SceneGraph* scene_graph, u32 camera_id, uint view) {
#ifdef ENABLE_DEBUG_LOGGING
    log.debug("Rendering full screen quad to view %d", view);
#endif
    // Bind vertex buffer and material, then submit.
    fullscreen_quad_->bind(r);
    material_->applyRendererState(Mat4::identity, Mat4::identity);
    r->submit(view, material_->program()->internalHandle(), 3);
}

RenderPipeline::PNode::PNode() {
}

void RenderPipeline::PNode::prepareForRendering(rhi::RHIRenderer* r, uint view) {
    r->setViewFrameBuffer(view, output_frame_buffer_ ? output_frame_buffer_->internalHandle()
                                                     : rhi::FrameBufferHandle{0});
}
}  // namespace dw
