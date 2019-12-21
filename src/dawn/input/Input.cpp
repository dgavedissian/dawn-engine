/*
    Dawn Engine
    Written by David Avedissian (c) 2012-2019 (git@dga.dev)
    Author: David Avedissian
*/
#include "Base.h"
#include "Input.h"

namespace dw {
Input::Input(Context* context)
    : Module(context),
      viewport_size_(1280, 800),
      mouse_position_(0, 0),
      mouse_move_(0, 0),
      mouse_scroll_(0.0f, 0.0f) {
    for (bool& i : key_down_) {
        i = false;
    }
    for (bool& i : mouse_button_state_) {
        i = false;
    }
}

Input::~Input() {
}

void Input::registerEventSystem(EventSystem* event_system) {
    event_systems_.emplace_back(event_system);
}

void Input::unregisterEventSystem(EventSystem* event_system) {
    auto it = std::find(event_systems_.begin(), event_systems_.end(), event_system);
    if (it != event_systems_.end()) {
        event_systems_.erase(it);
    }
}

bool Input::isKeyDown(Key::Enum key) const {
    return key_down_[key];
}

bool Input::isMouseButtonDown(MouseButton::Enum button) const {
    return mouse_button_state_[button];
}

Vec2i Input::mousePosition() const {
    return mouse_position_;
}

Vec2 Input::mousePositionRelative() const {
    Vec2i mouse_position = mousePosition();
    return {static_cast<float>(mouse_position.x) / viewport_size_.x,
            static_cast<float>(mouse_position.y) / viewport_size_.y};
}

Vec2i Input::mouseMove() const {
    return mouse_move_;
}

Vec2 Input::mouseScroll() const {
    return mouse_scroll_;
}

gfx::InputCallbacks Input::getGfxInputCallbacks() {
    gfx::InputCallbacks input_callbacks;
    static_assert(static_cast<int>(gfx::Key::Count) == Key::Count);
    static_assert(static_cast<int>(gfx::MouseButton::Count) == MouseButton::Count);
    input_callbacks.on_key = [this](gfx::Key::Enum key, gfx::Modifier::Enum modifier,
                                    bool pressed) mutable {
        onKey(static_cast<Key::Enum>(key), static_cast<Modifier::Enum>(modifier), pressed);
    };
    input_callbacks.on_char_input = [this](const std::string& input) mutable {
        onCharInput(input);
    };
    input_callbacks.on_mouse_button = [this](gfx::MouseButton::Enum button, bool pressed) mutable {
        onMouseButton(static_cast<MouseButton::Enum>(button), pressed);
    };
    input_callbacks.on_mouse_move = [this](const gfx::Vec2i& position) mutable {
        onMouseMove({position.x, position.y});
    };
    input_callbacks.on_mouse_scroll = [this](const Vec2& offset) mutable { onMouseScroll(offset); };
    return input_callbacks;
}

void Input::onKey(Key::Enum key, Modifier::Enum modifier, bool pressed) {
    key_down_[key] = pressed;
    for (auto es : event_systems_) {
        es->triggerEvent<KeyEvent>(key, modifier, pressed);
    }
}

void Input::onCharInput(const String& text) {
    for (auto es : event_systems_) {
        es->triggerEvent<CharInputEvent>(text);
    }
}

void Input::onMouseButton(MouseButton::Enum button, bool pressed) {
    mouse_button_state_[button] = pressed;
    for (auto es : event_systems_) {
        es->triggerEvent<MouseButtonEvent>(button, pressed);
    }
}

void Input::onMouseMove(const Vec2i& position) {
    mouse_move_ = position - mouse_position_;
    mouse_position_ = position;
    for (auto es : event_systems_) {
        es->triggerEvent<MouseMoveEvent>(mouse_position_, mousePositionRelative(), mouse_move_);
    }
}

void Input::onMouseScroll(const Vec2& offset) {
    mouse_scroll_ = offset;
    for (auto es : event_systems_) {
        es->triggerEvent<MouseScrollEvent>(offset);
    }
}
}  // namespace dw
