/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "input/Input.h"
#include "StateManager.h"

namespace dw {

StateManager::StateManager(Context* context) : Object(context) {
    addEventListener<EvtData_Key>(makeEventDelegate(this, &StateManager::onKey));
}

StateManager::~StateManager() {
    removeEventListener<EvtData_Key>(makeEventDelegate(this, &StateManager::onKey));

    while (!state_stack_.empty()) {
        pop();
    }
    state_map_.clear();
}

void StateManager::registerState(SharedPtr<State> state) {
    state_map_[state->id()] = state;
}

void StateManager::changeTo(StateId id) {
    if (state_stack_.size() > 0) {
        pop();
    }
    push(id);
}

void StateManager::push(StateId id) {
    if (id == S_NO_STATE) {
        return;
    }

    // TODO: We need some kind of way to "pause" the frame timer
    SharedPtr<State> newState = stateById(id);
    state_stack_.push_back(newState);
    newState->enter();
    log().info("Pushed %s", newState->name());
}

void StateManager::pop() {
    if (state_stack_.size() > 0) {
        SharedPtr<State> back = state_stack_.back();
        back->exit();
        state_stack_.pop_back();
        log().info("Popped %s", back->name());
    } else {
        // TODO(#21): Deal with this error correctly.
        log().error("Trying to pop a state when no states are on the stack");
    }
}

void StateManager::reload() {
    StateId top_id = top();
    pop();
    push(top_id);
}

void StateManager::clear() {
    while (state_stack_.size() > 0)
        pop();
}

void StateManager::update(float dt) {
    u64 size = state_stack_.size();
    for (uint i = 0; i < size; ++i) {
        state_stack_[i]->update(dt);
    }
}

void StateManager::preRender() {
    for (uint i = 0; i < state_stack_.size(); ++i) {
        state_stack_[i]->preRender();
    }
}

StateId StateManager::top() const {
    if (state_stack_.size() > 0) {
        return state_stack_.back()->id();
    }
    return S_NO_STATE;
}

SharedPtr<State> StateManager::stateById(StateId id) {
    auto it = state_map_.find(id);
    assert(it != state_map_.end());
    return (*it).second;
}

void StateManager::onKey(const EvtData_Key& data) {
    if (data.key == Key::F8 && data.down) {
        reload();
    }
}
}  // namespace dw
