/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "core/io/StringInputStream.h"
#include "input/Input.h"
#include "ui/Imgui.h"
#include "ui/UserInterface.h"

static_assert(sizeof(ImDrawIdx) == sizeof(dw::u16), "Only 16-bit ImGUI indices are supported.");

namespace dw {
UserInterface::UserInterface(Context* ctx) : Module(ctx), mouse_wheel_(0.0f) {
    setDependencies<Renderer>();
    setOptionalDependencies<Input>();

    logic_context_ = ImGui::CreateContext();
    renderer_context_ = ImGui::CreateContext();

    ImGui::SetCurrentContext(logic_context_);
    logic_io_ = &ImGui::GetIO();
    ImGui::SetCurrentContext(renderer_context_);
    renderer_io_ = &ImGui::GetIO();

    rhi_ = module<Renderer>()->rhi();

    // Initialise mouse state.
    for (bool& state : mouse_pressed_) {
        state = false;
    }

    forAllContexts([this](ImGuiIO& io) {
        // TODO: Resize this on screen size change.
        // TODO: Fill others settings of the io structure later.
        io.DisplaySize.x = rhi_->backbufferSize().x / rhi_->windowScale().x;
        io.DisplaySize.y = rhi_->backbufferSize().y / rhi_->windowScale().y;
        io.DisplayFramebufferScale.x = rhi_->windowScale().x;
        io.DisplayFramebufferScale.y = rhi_->windowScale().y;
        io.IniFilename = nullptr;

        // Load font texture atlas.
        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        rhi::TextureHandle handle =
            rhi_->createTexture2D(static_cast<u16>(width), static_cast<u16>(height),
                                  rhi::TextureFormat::RGBA8, pixels, width * height * 4);
        io.Fonts->TexID = reinterpret_cast<void*>(static_cast<uintptr>(handle.internal()));

        // Set up key map.
        io.KeyMap[ImGuiKey_Tab] = Key::Tab;
        io.KeyMap[ImGuiKey_LeftArrow] = Key::Left;
        io.KeyMap[ImGuiKey_RightArrow] = Key::Right;
        io.KeyMap[ImGuiKey_UpArrow] = Key::Up;
        io.KeyMap[ImGuiKey_DownArrow] = Key::Down;
        io.KeyMap[ImGuiKey_PageUp] = Key::PageUp;
        io.KeyMap[ImGuiKey_PageDown] = Key::PageDown;
        io.KeyMap[ImGuiKey_Home] = Key::Home;
        io.KeyMap[ImGuiKey_End] = Key::End;
        io.KeyMap[ImGuiKey_Delete] = Key::Delete;
        io.KeyMap[ImGuiKey_Backspace] = Key::Backspace;
        io.KeyMap[ImGuiKey_Enter] = Key::Enter;
        io.KeyMap[ImGuiKey_Escape] = Key::Escape;
        io.KeyMap[ImGuiKey_A] = Key::A;
        io.KeyMap[ImGuiKey_C] = Key::C;
        io.KeyMap[ImGuiKey_V] = Key::V;
        io.KeyMap[ImGuiKey_X] = Key::X;
        io.KeyMap[ImGuiKey_Y] = Key::Y;
        io.KeyMap[ImGuiKey_Z] = Key::Z;
    });

    // Set up renderer resources.
    vertex_decl_.begin()
        .add(rhi::VertexDecl::Attribute::Position, 2, rhi::VertexDecl::AttributeType::Float)
        .add(rhi::VertexDecl::Attribute::TexCoord0, 2, rhi::VertexDecl::AttributeType::Float)
        .add(rhi::VertexDecl::Attribute::Colour, 4, rhi::VertexDecl::AttributeType::Uint8, true)
        .end();
    auto vertex_shader_src = StringInputStream{R"(
        #version 330 core

        layout(location = 0) in vec2 position;
        layout(location = 1) in vec2 texcoord;
        layout(location = 2) in vec4 colour;

        uniform mat4 projection_matrix;

        out vec2 frag_texcoord;
        out vec4 frag_colour;

        void main()
        {
            frag_colour = colour;
            frag_texcoord = texcoord;
            gl_Position = projection_matrix * vec4(position, 0.0, 1.0);
        }
    )"};
    auto fragment_shader_src = StringInputStream{R"(
        #version 330 core

        in vec2 frag_texcoord;
        in vec4 frag_colour;

        uniform sampler2D ui_texture;

        layout(location = 0) out vec4 out_colour;

        void main()
        {
            out_colour = frag_colour * texture(ui_texture, frag_texcoord);
        }
    )"};
    auto vertex_shader = makeShared<VertexShader>(context());
    vertex_shader->load("imgui_shader.vs", vertex_shader_src);
    auto fragment_shader = makeShared<FragmentShader>(context());
    fragment_shader->load("imgui_shader.fs", fragment_shader_src);
    program_ = makeShared<Program>(context(), vertex_shader, fragment_shader);
    program_->setUniform<int>("ui_texture", 0);

    // Register delegates.
    addEventListener<KeyEvent>(makeEventDelegate(this, &UserInterface::onKey));
    addEventListener<CharInputEvent>(makeEventDelegate(this, &UserInterface::onCharInput));
    addEventListener<MouseButtonEvent>(makeEventDelegate(this, &UserInterface::onMouseButton));
    addEventListener<MouseScrollEvent>(makeEventDelegate(this, &UserInterface::onMouseScroll));
}

UserInterface::~UserInterface() {
    removeEventListener<KeyEvent>(makeEventDelegate(this, &UserInterface::onKey));
    removeEventListener<CharInputEvent>(makeEventDelegate(this, &UserInterface::onCharInput));
    removeEventListener<MouseButtonEvent>(makeEventDelegate(this, &UserInterface::onMouseButton));
    removeEventListener<MouseScrollEvent>(makeEventDelegate(this, &UserInterface::onMouseScroll));
}

void UserInterface::beginTick() {
    ImGui::SetCurrentContext(logic_context_);
    ImGui::NewFrame();
}

void UserInterface::endTick() {
    ImGui::Render();
}

void UserInterface::preRender() {
    ImGui::SetCurrentContext(renderer_context_);
    ImGui::NewFrame();
}

void UserInterface::postRender() {
    ImGui::Render();
}

void UserInterface::update(float dt) {
    forAllContexts([dt](ImGuiIO& io) { io.DeltaTime = dt; });
}

void UserInterface::render() {
    ImGui::SetCurrentContext(logic_context_);
    drawGUI(ImGui::GetDrawData(), *logic_io_);
    ImGui::SetCurrentContext(renderer_context_);
    drawGUI(ImGui::GetDrawData(), *renderer_io_);

    // Read input state and pass to imgui.
    auto* input = module<Input>();
    if (input) {
        // Update mouse button state and reset.
        bool left_state =
            mouse_pressed_[MouseButton::Left] || input->isMouseButtonDown(MouseButton::Left);
        bool right_state =
            mouse_pressed_[MouseButton::Right] || input->isMouseButtonDown(MouseButton::Right);
        bool middle_state =
            mouse_pressed_[MouseButton::Middle] || input->isMouseButtonDown(MouseButton::Middle);
        for (bool& state : mouse_pressed_) {
            state = false;
        }

        // Apply data to ImGui
        forAllContexts([this, input, left_state, right_state, middle_state](ImGuiIO& io) {
            // Mouse position and wheel.
            const auto& mouse_position = input->mousePosition();
            io.MousePos.x = mouse_position.x / io.DisplayFramebufferScale.x;
            io.MousePos.y = mouse_position.y / io.DisplayFramebufferScale.y;
            io.MouseWheel = mouse_wheel_;
            mouse_wheel_ = 0.0f;

            io.MouseDown[0] = left_state;
            io.MouseDown[1] = right_state;
            io.MouseDown[2] = middle_state;
        });
    }
}

void UserInterface::forAllContexts(Function<void(ImGuiIO& io)> functor) {
    functor(*logic_io_);
    functor(*renderer_io_);
}

void UserInterface::drawGUI(ImDrawData* draw_data, ImGuiIO& io) {
    if (!draw_data) {
        return;
    }

    // Give up if we have no draw surface.
    if (io.DisplaySize.x == 0.0f || io.DisplaySize.y == 0.0f) {
        return;
    }

    // Setup projection matrix.
    Mat4 projection_matrix =
        Mat4::OpenGLOrthoProjRH(-1.0f, 1.0f, io.DisplaySize.x, io.DisplaySize.y) *
        Mat4::Translate(-io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f, 0.0f) *
        Mat4::Scale(1.0f, -1.0f, 1.0f);
    program_->setUniform<Mat4>("projection_matrix", projection_matrix);

    for (int n = 0; n < draw_data->CmdListsCount; ++n) {
        auto cmd_list = draw_data->CmdLists[n];

        // Update GPU buffers.
        auto& vtx_buffer = cmd_list->VtxBuffer;
        auto& idx_buffer = cmd_list->IdxBuffer;
        auto tvb = rhi_->allocTransientVertexBuffer(vtx_buffer.Size, vertex_decl_);
        if (tvb == rhi::TransientVertexBufferHandle::invalid) {
            log().warn("Failed to allocate transient vertex buffer for ImGui");
            continue;
        }
        auto tib = rhi_->allocTransientIndexBuffer(idx_buffer.Size);
        if (tib == rhi::TransientIndexBufferHandle::invalid) {
            log().warn("Failed to allocate transient index buffer for ImGui");
            continue;
        }
        memcpy(rhi_->getTransientVertexBufferData(tvb), vtx_buffer.Data,
               vtx_buffer.Size * sizeof(ImDrawVert));
        memcpy(rhi_->getTransientIndexBufferData(tib), idx_buffer.Data,
               idx_buffer.Size * sizeof(ImDrawIdx));

        // Execute draw commands.
        uint offset = 0;
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; ++cmd_i) {
            const ImDrawCmd* cmd = &cmd_list->CmdBuffer[cmd_i];
            if (cmd->UserCallback) {
                cmd->UserCallback(cmd_list, cmd);
            } else {
                // Set render state.
                rhi_->setStateEnable(rhi::RenderState::Blending);
                rhi_->setStateBlendEquation(rhi::BlendEquation::Add, rhi::BlendFunc::SrcAlpha,
                                            rhi::BlendFunc::OneMinusSrcAlpha);
                rhi_->setStateDisable(rhi::RenderState::CullFace);
                rhi_->setStateDisable(rhi::RenderState::Depth);
                rhi_->setScissor(static_cast<u16>(cmd->ClipRect.x * io.DisplayFramebufferScale.x),
                                 static_cast<u16>(cmd->ClipRect.y * io.DisplayFramebufferScale.y),
                                 static_cast<u16>((cmd->ClipRect.z - cmd->ClipRect.x) *
                                                  io.DisplayFramebufferScale.x),
                                 static_cast<u16>((cmd->ClipRect.w - cmd->ClipRect.y) *
                                                  io.DisplayFramebufferScale.y));

                // Set resources.
                rhi_->setTexture(rhi::TextureHandle{static_cast<rhi::TextureHandle::base_type>(
                                     reinterpret_cast<uintptr>(cmd->TextureId))},
                                 0);
                rhi_->setVertexBuffer(tvb);
                rhi_->setIndexBuffer(tib);

                // Draw.
                program_->applyRendererState();
                rhi_->submit(rhi_->backbufferView(), program_->internalHandle(), cmd->ElemCount,
                             offset);
            }
            offset += cmd->ElemCount;
        }
    }
}

void UserInterface::onKey(const KeyEvent& state) {
    forAllContexts([state](ImGuiIO& io) {
        io.KeysDown[state.key] = state.down;
        io.KeyCtrl = io.KeysDown[Key::LeftCtrl] || io.KeysDown[Key::RightCtrl];
        io.KeyShift = io.KeysDown[Key::LeftShift] || io.KeysDown[Key::RightShift];
        io.KeyAlt = io.KeysDown[Key::LeftAlt] || io.KeysDown[Key::RightAlt];
        io.KeySuper = io.KeysDown[Key::LeftSuper] || io.KeysDown[Key::RightSuper];
    });
}

void UserInterface::onCharInput(const CharInputEvent& text) {
    forAllContexts([text](ImGuiIO& io) { io.AddInputCharactersUTF8(text.text.c_str()); });
}

void UserInterface::onMouseButton(const MouseButtonEvent& mouse_button) {
    mouse_pressed_[mouse_button.button] = mouse_button.down;
}

void UserInterface::onMouseScroll(const MouseScrollEvent& scroll) {
    mouse_wheel_ += scroll.motion.y;
}
}  // namespace dw
