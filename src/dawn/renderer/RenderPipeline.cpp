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

namespace dw {
RenderPipeline::RenderPipeline(Context* ctx) : Resource{ctx} {
}

SharedPtr<RenderPipeline> RenderPipeline::createFromDesc(Context* ctx,
                                                         const RenderPipelineDesc& desc) {
    auto& log = ctx->module<Logger>()->withObjectName("dw::RenderPipeline");

    // Verify that the pipeline nodes fit together.
    for (auto& node_instance : desc.pipeline) {
        auto node_it = desc.nodes.find(node_instance.node);
        if (node_it == desc.nodes.end()) {
            auto error = tfm::format("Node '%s' does not exist.", node_instance.node);
            log.error(error);
            return nullptr;
        }

        auto& node = node_it->second;

        // Verify that all inputs and outputs are bound.
        if (node.inputs.size() != node_instance.input_bindings.size()) {
            auto error = tfm::format(
                "Mismatching input bindings. Number of input is %d but number of bindings is %d.",
                node.inputs.size(), node_instance.input_bindings.size());
            log.error(error);
            return nullptr;
        }
        if (node.outputs.size() != node_instance.output_bindings.size()) {
            auto error = tfm::format(
                "Mismatching output bindings. Number of outputs is %d but number of bindings is "
                "%d.",
                node.outputs.size(), node_instance.output_bindings.size());
            log.error(error);
            return nullptr;
        }

        // Verify that the bindings make sense.
        HashSet<String> inputs_bound;
        HashSet<String> textures_bound_to_inputs;
        for (auto& binding : node_instance.input_bindings) {
            auto input_it = node.inputs.find(binding.first);
            if (input_it == node.inputs.end()) {
                auto error = tfm::format("Input '%s' does not exist.", binding.first);
                log.error(error);
                return nullptr;
            }

            if (inputs_bound.count(binding.first) > 0) {
                auto error = tfm::format("Input '%s' is already bound.", binding.first);
                log.error(error);
                return nullptr;
            }

            if (textures_bound_to_inputs.count(binding.second) > 0) {
                auto error = tfm::format("Texture '%s' is already bound.", binding.second);
                log.error(error);
                return nullptr;
            }

            auto texture_it = desc.textures.find(binding.second);
            if (texture_it == desc.textures.end()) {
                auto error = tfm::format("Texture '%s' bound to '%s' doesn't exist.",
                                         binding.second, binding.first);
                log.error(error);
                return nullptr;
            }

            if (input_it->second != texture_it->second.format) {
                auto error =
                    tfm::format("Texture format mismatch. Input: %s (%d). Texture: %s (%d)",
                                input_it->first, static_cast<int>(input_it->second),
                                texture_it->first, static_cast<int>(texture_it->second.format));
                log.error(error);
                return nullptr;
            }
        }
        HashSet<String> outputs_bound;
        HashSet<String> textures_bound_to_outputs;
        for (auto& binding : node_instance.output_bindings) {
            auto output_it =
                std::find_if(node.outputs.begin(), node.outputs.end(),
                             [&binding](const Pair<String, rhi::TextureFormat>& item) -> bool {
                                 return item.first == binding.first;
                             });
            if (output_it == node.outputs.end()) {
                auto error = tfm::format("Output '%s' does not exist.", binding.first);
                log.error(error);
                return nullptr;
            }

            if (outputs_bound.count(binding.first) > 0) {
                auto error = tfm::format("Output '%s' is already bound.", binding.first);
                log.error(error);
                return nullptr;
            }

            if (textures_bound_to_outputs.count(binding.second) > 0) {
                auto error = tfm::format("Texture '%s' is already bound.", binding.second);
                log.error(error);
                return nullptr;
            }

            if (binding.second == RenderPipelineDesc::PipelineOutput) {
                if (output_it->second != rhi::TextureFormat::RGBA8) {
                    auto error = tfm::format(
                        "Texture format mismatch. Output: %s (%d). Texture: Output (RGBA8)",
                        output_it->first, static_cast<int>(output_it->second));
                    log.error(error);
                    return nullptr;
                }
            } else {
                auto texture_it = desc.textures.find(binding.second);
                if (texture_it == desc.textures.end()) {
                    auto error = tfm::format("Texture '%s' bound to '%s' doesn't exist.",
                                             binding.second, binding.first);
                    log.error(error);
                    return nullptr;
                }

                if (output_it->second != texture_it->second.format) {
                    auto error =
                        tfm::format("Texture format mismatch. Output: %s (%d). Texture: %s (%d)",
                                    output_it->first, static_cast<int>(output_it->second),
                                    texture_it->first, static_cast<int>(texture_it->second.format));
                    log.error(error);
                    return nullptr;
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
    HashMap<String, SharedPtr<Texture>> textures;
    for (auto& texture_desc : desc.textures) {
        textures[texture_desc.first] = Texture::createTexture2D(
            ctx, ctx->module<Renderer>()->rhi()->backbufferSize() * texture_desc.second.ratio,
            texture_desc.second.format);
    }

    // Build nodes.
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
                step = makeUnique<PRenderQuadStep>(render_pipeline->fullscreen_quad_,
                                                   ctx->module<ResourceCache>()->get<Material>(
                                                       render_quad_step_desc.material_name),
                                                   node->input_samplers_);
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

bool RenderPipeline::beginLoad(const String& asset_name, InputStream& src) {
    return false;
}

void RenderPipeline::endLoad() {
}

void RenderPipeline::render(float interpolation, SceneGraph* scene_graph, u32 camera_id) {
    auto rhi = module<Renderer>()->rhi();
    for (uint view = 0; view < nodes_.size(); ++view) {
        nodes_[view]->prepareForRendering(rhi, view);
    }
    for (uint view = 0; view < nodes_.size(); ++view) {
        for (auto& step : nodes_[view]->steps_) {
            step->execute(log(), rhi, interpolation, scene_graph, camera_id, view);
        }
    }
}

RenderPipeline::PClearStep::PClearStep(Colour colour) : colour_(colour) {
}

void RenderPipeline::PClearStep::execute(Logger& log, rhi::Renderer* r, float interpolation,
                                         SceneGraph* scene_graph, u32 camera_id, uint view) {
    log.debug("Setting view clear to %s", colour_.rgba().ToString());
    r->setViewClear(view, colour_);
}

RenderPipeline::PRenderQueueStep::PRenderQueueStep(u32 mask) : mask_(mask) {
}

void RenderPipeline::PRenderQueueStep::execute(Logger& log, rhi::Renderer* r, float interpolation,
                                               SceneGraph* scene_graph, u32 camera_id, uint view) {
    log.debug("Rendering scene from camera %d (mask: 0x%x) to view %d", camera_id, mask_, view);
    scene_graph->renderSceneFromCamera(interpolation, camera_id, view, mask_);
}

RenderPipeline::PRenderQuadStep::PRenderQuadStep(SharedPtr<VertexBuffer> fullscreen_quad,
                                                 SharedPtr<Material> material,
                                                 const HashMap<String, uint>& input_samplers)
    : fullscreen_quad_(std::move(fullscreen_quad)),
      material_(std::move(material)),
      input_samplers_(input_samplers) {
}

void RenderPipeline::PRenderQuadStep::execute(Logger& log, rhi::Renderer* r, float interpolation,
                                              SceneGraph* scene_graph, u32 camera_id, uint view) {
    log.debug("Rendering full screen quad to view %d", view);
    // Set up inputs.
    for (auto& input : input_samplers_) {
        r->setUniform(input.first + "_sampler", static_cast<int>(input.second));
    }

    // Bind vertex buffer and material, then submit.
    fullscreen_quad_->bind(r);
    material_->applyRendererState(Mat4::identity, Mat4::identity);
    r->submit(view, material_->program()->internalHandle(), 3);
}

RenderPipeline::PNode::PNode() {
}

void RenderPipeline::PNode::prepareForRendering(rhi::Renderer* r, uint view) {
    r->setViewFrameBuffer(view, output_frame_buffer_ ? output_frame_buffer_->internalHandle()
                                                     : rhi::FrameBufferHandle{0});
}
}  // namespace dw
