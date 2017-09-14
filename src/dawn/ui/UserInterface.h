/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Renderer.h"
#include "renderer/Program.h"
#include "ui/Imgui.h"

namespace dw {
class DW_API UserInterface : public Object {
public:
    DW_OBJECT(UserInterface);

    UserInterface(Context* ctx);
    ~UserInterface();

    void update(float dt);
    void render();

private:
    Renderer* renderer_;

    // ImGui.
    ImGuiIO& imgui_io_;
    ImGuiStyle& imgui_style_;
    SharedPtr<Program> program_;
    VertexDecl vertex_decl_;

    // Input.
    float mouse_wheel_;
    bool mouse_pressed_[MouseButton::Count];

    // Callback handlers.
    void onKey(const EvtData_Key& state);
    void onCharInput(const EvtData_CharInput& text);
    void onMouseButton(const EvtData_MouseButton& mouse_button);
    void onMouseScroll(const EvtData_MouseScroll& scroll);
};
}  // namespace dw
