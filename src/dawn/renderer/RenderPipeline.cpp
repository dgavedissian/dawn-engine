/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "resource/Resource.h"
#include "renderer/Texture.h"
#include "renderer/FrameBuffer.h"
#include "renderer/RenderPipeline.h"
#include "renderer/CustomRenderable.h"
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
    // Verify that the pipeline nodes fit together.
    for (auto& node_instance : desc.pipeline) {
        auto node_it = desc.nodes.find(node_instance.node);
        if (node_it == desc.nodes.end()) {
            return makeError(str::format("Node '{}' does not exist.", node_instance.node));
        }

        auto& node = node_it->second;

        // Verify that all inputs and outputs are bound.
        if (node.inputs.size() != node_instance.input_bindings.size()) {
            return makeError(str::format(
                "Mismatching input bindings. Number of input is {} but number of bindings is {}.",
                node.inputs.size(), node_instance.input_bindings.size()));
        }
        if (node.outputs.size() != node_instance.output_bindings.size()) {
            return makeError(str::format(
                "Mismatching output bindings. Number of outputs is {} but number of bindings is "
                "{}.",
                node.outputs.size(), node_instance.output_bindings.size()));
        }

        // Verify that the bindings make sense.
        HashSet<String> inputs_bound;
        HashSet<String> textures_bound_to_inputs;
        for (auto& binding : node_instance.input_bindings) {
            auto input_it = node.inputs.find(binding.first);
            if (input_it == node.inputs.end()) {
                return makeError(str::format("Input '{}' does not exist.", binding.first));
            }

            if (inputs_bound.count(binding.first) > 0) {
                return makeError(str::format("Input '{}' is already bound.", binding.first));
            }

            if (textures_bound_to_inputs.count(binding.second) > 0) {
                return makeError(str::format("Texture '{}' is already bound.", binding.second));
            }

            auto texture_it = desc.textures.find(binding.second);
            if (texture_it == desc.textures.end()) {
                return makeError(str::format("Texture '{}' bound to '{}' doesn't exist.",
                                             binding.second, binding.first));
            }

            if (input_it->second != texture_it->second.format) {
                return makeError(
                    str::format("Texture format mismatch. Input: {} ({}). Texture: {} ({})",
                                input_it->first, static_cast<int>(input_it->second),
                                texture_it->first, static_cast<int>(texture_it->second.format)));
            }
        }
        HashSet<String> outputs_bound;
        HashSet<String> textures_bound_to_outputs;
        Option<gfx::TextureFormat> output_format;
        for (auto& binding : node_instance.output_bindings) {
            auto output_it =
                std::find_if(node.outputs.begin(), node.outputs.end(),
                             [&binding](const Pair<String, gfx::TextureFormat>& item) -> bool {
                                 return item.first == binding.first;
                             });
            if (output_it == node.outputs.end()) {
                return makeError(str::format("Output '{}' does not exist.", binding.first));
            }

            if (outputs_bound.count(binding.first) > 0) {
                return makeError(str::format("Output '{}' is already bound.", binding.first));
            }

            if (textures_bound_to_outputs.count(binding.second) > 0) {
                return makeError(str::format("Texture '{}' is already bound.", binding.second));
            }

            // As all outputs are bound to a single render target (called a multiple render target),
            // we should ensure that the formats are identical.
            if (output_format.has_value()) {
                if (output_it->second != *output_format) {
                    return makeError(str::format(
                        "Texture format mismatch. Invalid MRT. Output {} is {}, but expecting {}.",
                        output_it->first, static_cast<int>(output_it->second),
                        static_cast<int>(*output_format)));
                }
            } else {
                output_format = output_it->second;
            }

            if (binding.second == RenderPipelineDesc::PipelineOutput) {
                if (output_it->second != gfx::TextureFormat::RGBA8) {
                    return makeError(str::format(
                        "Texture format mismatch. Output: {} ({}). Texture: Output (RGBA8)",
                        output_it->first, static_cast<int>(output_it->second)));
                }
            } else {
                auto texture_it = desc.textures.find(binding.second);
                if (texture_it == desc.textures.end()) {
                    return makeError(str::format("Texture '{}' bound to '{}' doesn't exist.",
                                                 binding.second, binding.first));
                }

                if (output_it->second != texture_it->second.format) {
                    return makeError(str::format(
                        "Texture format mismatch. Output: {} ({}). Texture: {} ({})",
                        output_it->first, static_cast<int>(output_it->second), texture_it->first,
                        static_cast<int>(texture_it->second.format)));
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
    gfx::VertexDecl decl;
    decl.begin()
        .add(gfx::VertexDecl::Attribute::Position, 2, gfx::VertexDecl::AttributeType::Float)
        .add(gfx::VertexDecl::Attribute::TexCoord0, 2, gfx::VertexDecl::AttributeType::Float)
        .end();
    render_pipeline->fullscreen_quad_ =
        makeShared<VertexBuffer>(ctx, gfx::Memory(vertices, sizeof(vertices)), 3, decl);

    // Create textures.
    for (auto& texture_desc : desc.textures) {
        auto bb_size = ctx->module<Renderer>()->rhi()->backbufferSize();
        render_pipeline->textures_[texture_desc.first] =
            Texture::createTexture2D(ctx, Vec2i{bb_size.x, bb_size.y} * texture_desc.second.ratio,
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
            Result<UniquePtr<PStep>, String> step_result;
            if (holdsAlternative<RenderPipelineDesc::ClearStep>(step_desc)) {
                auto& clear_step = get<RenderPipelineDesc::ClearStep>(step_desc);
                step_result = {makeUnique<PClearStep>(clear_step.colour)};
            } else if (holdsAlternative<RenderPipelineDesc::RenderQueueStep>(step_desc)) {
                auto& render_queue_step = get<RenderPipelineDesc::RenderQueueStep>(step_desc);
                step_result = {makeUnique<PRenderQueueStep>(render_queue_step.mask)};
            } else if (holdsAlternative<RenderPipelineDesc::RenderQuadStep>(step_desc)) {
                auto& render_quad_step = get<RenderPipelineDesc::RenderQuadStep>(step_desc);
                auto material =
                    ctx->module<ResourceCache>()->get<Material>(render_quad_step.material_name);
                if (!material) {
                    return makeError(
                        str::format("Unable to set up material in render quad step. Reason: {}",
                                    material.error()));
                }
                auto material_instance = makeShared<Material>(**material);
                for (auto& sampler : node->input_samplers_) {
                    material_instance->setUniform<int>(sampler.first + "_sampler", sampler.second);
                    material_instance->setTexture(node->input_textures_.at(sampler.first),
                                                  sampler.second);
                }
                step_result = {makeUnique<PRenderQuadStep>(
                    render_pipeline->fullscreen_quad_, material_instance, node->input_samplers_)};
            }
            if (!step_result) {
                return makeError(step_result.error());
            }
            node->steps_.push_back(std::move(*step_result));
        }

        // Create output frame buffer.
        node->output_frame_buffer_ =
            backbuffer_output ? nullptr : makeUnique<FrameBuffer>(ctx, node->output_textures_);

        // Add node.
        render_pipeline->nodes_.push_back(std::move(node));
    }

    return render_pipeline;
}

Result<void> RenderPipeline::beginLoad(const String& asset_name, InputStream& src) {
    return makeError("Render pipeline loading unimplemented.");
}

void RenderPipeline::render(float dt, float interpolation, SceneGraph* scene_graph, u32 camera_id) {
    auto rhi = module<Renderer>()->rhi();
    for (uint view = 0; view < nodes_.size(); ++view) {
        nodes_[view]->prepareForRendering(rhi, view);
    }
    for (uint view = 0; view < nodes_.size(); ++view) {
        for (auto& step : nodes_[view]->steps_) {
            step->execute(log().withObjectName("dw::RenderPipeline"), rhi, dt, interpolation,
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

void RenderPipeline::PClearStep::execute(Logger& log, gfx::Renderer* r, float dt,
                                         float interpolation, SceneGraph* scene_graph,
                                         u32 camera_id, uint view) {
#ifdef ENABLE_DEBUG_LOGGING
    log.debug("Setting view clear to {}", colour_.rgba().ToString());
#endif
    r->setViewClear(view, colour_);
}

RenderPipeline::PRenderQueueStep::PRenderQueueStep(u32 mask) : mask_(mask) {
}

void RenderPipeline::PRenderQueueStep::execute(Logger& log, gfx::Renderer* r, float dt,
                                               float interpolation, SceneGraph* scene_graph,
                                               u32 camera_id, uint view) {
#ifdef ENABLE_DEBUG_LOGGING
    log.debug("Rendering scene from camera {} (mask: {:#x}) to view {}", camera_id, mask_, view);
#endif
    scene_graph->renderSceneFromCamera(dt, interpolation, camera_id, view, mask_);
}

RenderPipeline::PRenderQuadStep::PRenderQuadStep(SharedPtr<VertexBuffer> fullscreen_quad,
                                                 SharedPtr<Material> material,
                                                 const HashMap<String, uint>& input_samplers)
    : fullscreen_quad_(std::move(fullscreen_quad)),
      material_(std::move(material)),
      input_samplers_(input_samplers) {
}

void RenderPipeline::PRenderQuadStep::execute(Logger& log, gfx::Renderer* r, float dt,
                                              float interpolation, SceneGraph* scene_graph,
                                              u32 camera_id, uint view) {
#ifdef ENABLE_DEBUG_LOGGING
    log.debug("Rendering full screen quad to view {}", view);
#endif
    // Bind vertex buffer and material, then submit.
    fullscreen_quad_->bind(r);
    material_->applyRendererState(Mat4::identity, Mat4::identity);
    r->submit(view, material_->program()->internalHandle(), 3);
}

RenderPipeline::PNode::PNode() {
}

void RenderPipeline::PNode::prepareForRendering(gfx::Renderer* r, uint view) {
    r->setViewFrameBuffer(view, output_frame_buffer_ ? output_frame_buffer_->internalHandle()
                                                     : gfx::FrameBufferHandle{0});
}
}  // namespace dw
