/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Renderer.h"
#include "renderer/Program.h"
#include "ui/Imgui.h"

namespace dw {
class DW_API UserInterface : public Subsystem {
public:
    DW_OBJECT(UserInterface);

    UserInterface(Context* ctx);
    ~UserInterface();

	void beginTick();
	void endTick();
	void preRender();
	void postRender();

    void update(float dt);
    void render();

private:
    Renderer* renderer_;

    // ImGui.
	ImGuiContext* logic_context_;
	ImGuiContext* renderer_context_;
	ImGuiIO* logic_io_;
	ImGuiIO* renderer_io_;
    SharedPtr<Program> program_;
    VertexDecl vertex_decl_;

    // Input.
	float mouse_wheel_;
    bool mouse_pressed_[MouseButton::Count];

	void forAllContexts(Function<void(ImGuiIO& io)> functor);
    void drawGUI(ImDrawData* draw_data, ImGuiIO& io);

    // Callback handlers.
    void onKey(const KeyEvent& state);
    void onCharInput(const CharInputEvent& text);
    void onMouseButton(const MouseButtonEvent& mouse_button);
    void onMouseScroll(const MouseScrollEvent& scroll);
};
}  // namespace dw
