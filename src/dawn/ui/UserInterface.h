/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "renderer/rhi/RHIRenderer.h"
#include "renderer/Program.h"
#include "input/Input.h"
#include "ui/Imgui.h"

namespace dw {
class DW_API UserInterface : public Object {
public:
    DW_OBJECT(UserInterface);

    UserInterface(Context* ctx, EventSystem* event_system);
    ~UserInterface();

    void preUpdate() const;
    void postUpdate() const;
    void preRender() const;
    void postRender() const;

    void update(float dt);
    void render();

private:
    rhi::RHIRenderer* rhi_;
    EventSystem* event_system_;

    // ImGui.
    ImGuiContext* logic_context_;
    ImGuiContext* renderer_context_;
    ImGuiIO* logic_io_;
    ImGuiIO* renderer_io_;
    SharedPtr<Program> program_;
    rhi::VertexDecl vertex_decl_;

    // Input.
    float mouse_wheel_;
    bool mouse_pressed_[MouseButton::Count];

    void forAllContexts(const Function<void(ImGuiIO& io)>& functor) const;
    void drawGUI(ImDrawData* draw_data, ImGuiIO& io) const;

    // Callback handlers.
    void onKey(const KeyEvent& state);
    void onCharInput(const CharInputEvent& text);
    void onMouseButton(const MouseButtonEvent& mouse_button);
    void onMouseScroll(const MouseScrollEvent& scroll);
};
}  // namespace dw
