/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "io/StringInputStream.h"
#include "input/Input.h"
#include "ui/Imgui.h"
#include "ui/UserInterface.h"

static_assert(sizeof(ImDrawIdx) == sizeof(dw::u16), "Only 16-bit ImGUI indices are supported.");

namespace dw {
UserInterface::UserInterface(Context* ctx)
    : Object(ctx),
      renderer_(subsystem<Renderer>()),
      imgui_io_(ImGui::GetIO()),
      imgui_style_(ImGui::GetStyle()),
      mouse_wheel_(0.0f) {
    // Initialise mouse state.
    for (bool& state : mouse_pressed_) {
        state = false;
    }

    // TODO: Resize this on screen size change.
    // TODO: Fill others settings of the io structure later.
    imgui_io_.DisplaySize.x = renderer_->getBackbufferSize().x;
    imgui_io_.DisplaySize.y = renderer_->getBackbufferSize().y;
    imgui_io_.RenderDrawListsFn = nullptr;
    imgui_io_.IniFilename = nullptr;

    // Load font texture atlas.
    unsigned char* pixels;
    int width, height;
    imgui_io_.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    TextureHandle handle =
        renderer_->createTexture2D(width, height, TextureFormat::RGBA8, pixels, width * height * 4);
    imgui_io_.Fonts->TexID = reinterpret_cast<void*>(static_cast<uintptr>(handle.internal()));

    // Set up key map.
    imgui_io_.KeyMap[ImGuiKey_Tab] = Key::Tab;
    imgui_io_.KeyMap[ImGuiKey_LeftArrow] = Key::Left;
    imgui_io_.KeyMap[ImGuiKey_RightArrow] = Key::Right;
    imgui_io_.KeyMap[ImGuiKey_UpArrow] = Key::Up;
    imgui_io_.KeyMap[ImGuiKey_DownArrow] = Key::Down;
    imgui_io_.KeyMap[ImGuiKey_PageUp] = Key::PageUp;
    imgui_io_.KeyMap[ImGuiKey_PageDown] = Key::PageDown;
    imgui_io_.KeyMap[ImGuiKey_Home] = Key::Home;
    imgui_io_.KeyMap[ImGuiKey_End] = Key::End;
    imgui_io_.KeyMap[ImGuiKey_Delete] = Key::Delete;
    imgui_io_.KeyMap[ImGuiKey_Backspace] = Key::Backspace;
    imgui_io_.KeyMap[ImGuiKey_Enter] = Key::Return;
    imgui_io_.KeyMap[ImGuiKey_Escape] = Key::Esc;
    imgui_io_.KeyMap[ImGuiKey_A] = Key::KeyA;
    imgui_io_.KeyMap[ImGuiKey_C] = Key::KeyC;
    imgui_io_.KeyMap[ImGuiKey_V] = Key::KeyV;
    imgui_io_.KeyMap[ImGuiKey_X] = Key::KeyX;
    imgui_io_.KeyMap[ImGuiKey_Y] = Key::KeyY;
    imgui_io_.KeyMap[ImGuiKey_Z] = Key::KeyZ;

    // Set up renderer resources.
    vertex_decl_.begin()
        .add(VertexDecl::Attribute::Position, 2, VertexDecl::AttributeType::Float)
        .add(VertexDecl::Attribute::TexCoord0, 2, VertexDecl::AttributeType::Float)
        .add(VertexDecl::Attribute::Colour, 4, VertexDecl::AttributeType::Uint8, true)
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

    // Begin a new frame.
    ImGui::NewFrame();

    // Register delegates.
    addEventListener<EvtData_Key>(makeEventDelegate(this, &UserInterface::onKey));
    addEventListener<EvtData_CharInput>(makeEventDelegate(this, &UserInterface::onCharInput));
    addEventListener<EvtData_MouseButton>(makeEventDelegate(this, &UserInterface::onMouseButton));
    addEventListener<EvtData_MouseScroll>(makeEventDelegate(this, &UserInterface::onMouseScroll));
}

UserInterface::~UserInterface() {
    removeEventListener<EvtData_Key>(makeEventDelegate(this, &UserInterface::onKey));
    removeEventListener<EvtData_CharInput>(makeEventDelegate(this, &UserInterface::onCharInput));
    removeEventListener<EvtData_MouseButton>(
        makeEventDelegate(this, &UserInterface::onMouseButton));
    removeEventListener<EvtData_MouseScroll>(
        makeEventDelegate(this, &UserInterface::onMouseScroll));
}

void UserInterface::update(float dt) {
    // Read input state and pass to imgui.
    auto input = subsystem<Input>();

    imgui_io_.DeltaTime = dt;

    // Mouse position and wheel.
    const auto& mouse_position = input->mousePosition();
    imgui_io_.MousePos.x = mouse_position.x;
    imgui_io_.MousePos.y = mouse_position.y;
    imgui_io_.MouseWheel = mouse_wheel_;
    mouse_wheel_ = 0.0f;

    // Pass mouse button state and reset.
    imgui_io_.MouseDown[0] =
        mouse_pressed_[MouseButton::Left] || input->isMouseButtonDown(MouseButton::Left);
    imgui_io_.MouseDown[1] =
        mouse_pressed_[MouseButton::Right] || input->isMouseButtonDown(MouseButton::Right);
    imgui_io_.MouseDown[2] =
        mouse_pressed_[MouseButton::Middle] || input->isMouseButtonDown(MouseButton::Middle);
    for (bool& state : mouse_pressed_) {
        state = false;
    }
}

void UserInterface::render() {
    // Generate ImGui render buffers and draw.
    ImGui::Render();
    auto draw_data = ImGui::GetDrawData();

    // Setup projection matrix.
    Mat4 projection_matrix =
        Mat4::OpenGLOrthoProjRH(-1.0f, 1.0f, imgui_io_.DisplaySize.x, imgui_io_.DisplaySize.y) *
        Mat4::Translate(-imgui_io_.DisplaySize.x * 0.5f, imgui_io_.DisplaySize.y * 0.5f, 0.0f) *
        Mat4::Scale(1.0f, -1.0f, 1.0f);
    program_->setUniform<Mat4>("projection_matrix", projection_matrix);

    for (int n = 0; n < draw_data->CmdListsCount; ++n) {
        auto cmd_list = draw_data->CmdLists[n];

        // Update GPU buffers.
        auto& vtx_buffer = cmd_list->VtxBuffer;
        auto& idx_buffer = cmd_list->IdxBuffer;
        auto tvb = renderer_->allocTransientVertexBuffer(vtx_buffer.Size, vertex_decl_);
        if (tvb == TransientVertexBufferHandle::invalid) {
            log().warn("Failed to allocate transient vertex buffer for ImGui");
            continue;
        }
        auto tib = renderer_->allocTransientIndexBuffer(idx_buffer.Size);
        if (tib == TransientIndexBufferHandle::invalid) {
            log().warn("Failed to allocate transient index buffer for ImGui");
            continue;
        }
        memcpy(renderer_->getTransientVertexBufferData(tvb), vtx_buffer.Data,
               vtx_buffer.Size * sizeof(ImDrawVert));
        memcpy(renderer_->getTransientIndexBufferData(tib), idx_buffer.Data,
               idx_buffer.Size * sizeof(ImDrawIdx));

        // Execute draw commands.
        uint offset = 0;
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; ++cmd_i) {
            const ImDrawCmd* cmd = &cmd_list->CmdBuffer[cmd_i];
            if (cmd->UserCallback) {
                cmd->UserCallback(cmd_list, cmd);
            } else {
                // Set render state.
                renderer_->setStateEnable(RenderState::Blending);
                renderer_->setStateBlendEquation(BlendEquation::Add, BlendFunc::SrcAlpha,
                                                 BlendFunc::OneMinusSrcAlpha);
                renderer_->setStateDisable(RenderState::CullFace);
                renderer_->setStateDisable(RenderState::Depth);
                renderer_->setScissor(static_cast<u16>(cmd->ClipRect.x),
                                      static_cast<u16>(cmd->ClipRect.y),
                                      static_cast<u16>(cmd->ClipRect.z - cmd->ClipRect.x),
                                      static_cast<u16>(cmd->ClipRect.w - cmd->ClipRect.y));

                // Set resources.
                renderer_->setTexture(TextureHandle{static_cast<TextureHandle::base_type>(
                                          reinterpret_cast<uintptr>(cmd->TextureId))},
                                      0);
                renderer_->setVertexBuffer(tvb);
                renderer_->setIndexBuffer(tib);

                // Draw.
                program_->prepareForRendering();
                renderer_->submit(0, program_->internalHandle(), cmd->ElemCount, offset);
            }
            offset += cmd->ElemCount;
        }
    }

    // Begin a new frame.
    ImGui::NewFrame();
}

void UserInterface::onKey(const EvtData_Key& state) {
    imgui_io_.KeysDown[state.key] = state.down;
    imgui_io_.KeyCtrl =
        imgui_io_.KeysDown[GLFW_KEY_LEFT_CONTROL] || imgui_io_.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    imgui_io_.KeyShift =
        imgui_io_.KeysDown[GLFW_KEY_LEFT_SHIFT] || imgui_io_.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    imgui_io_.KeyAlt =
        imgui_io_.KeysDown[GLFW_KEY_LEFT_ALT] || imgui_io_.KeysDown[GLFW_KEY_RIGHT_ALT];
    imgui_io_.KeySuper =
        imgui_io_.KeysDown[GLFW_KEY_LEFT_SUPER] || imgui_io_.KeysDown[GLFW_KEY_RIGHT_SUPER];
}

void UserInterface::onCharInput(const EvtData_CharInput& text) {
    imgui_io_.AddInputCharactersUTF8(text.text.c_str());
}

void UserInterface::onMouseButton(const EvtData_MouseButton& mouse_button) {
    mouse_pressed_[mouse_button.button] = mouse_button.down;
}

void UserInterface::onMouseScroll(const EvtData_MouseScroll& scroll) {
    mouse_wheel_ += scroll.motion.y;
}
}  // namespace dw
