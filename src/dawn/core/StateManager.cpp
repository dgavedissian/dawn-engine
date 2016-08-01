/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "input/Input.h"
#include "StateManager.h"

namespace dw {

StateManager::StateManager(Context* context) : Object(context) {
    ADD_LISTENER(StateManager, EvtData_KeyDown);
}

StateManager::~StateManager() {
    REMOVE_LISTENER(StateManager, EvtData_KeyDown);

    while (!mStateStack.empty())
        pop();
    mStateMap.clear();
}

void StateManager::registerState(SharedPtr<State> state) {
    mStateMap[state->getId()] = state;
}

void StateManager::changeTo(int id) {
    if (mStateStack.size() > 0) pop();
    push(id);
}

void StateManager::push(int id) {
    if (id == S_NO_STATE) return;

    // TODO: We need some kind of way to "pause" the frame timer
    SharedPtr<State> newState = getStateById(id);
    mStateStack.push_back(newState);
    newState->enter();
    LOG << "Pushed " << newState->getName();
}

void StateManager::pop() {
    if (mStateStack.size() > 0) {
        SharedPtr<State> back = mStateStack.back();
        back->exit();
        mStateStack.pop_back();
        LOG << "Popped " << back->getName();
    } else {
        LOG << "WARNING: Trying to pop a state when no states are on the stack";
    }
}

void StateManager::reload() {
    int top = getTop();
    pop();
    push(top);
}

void StateManager::clear() {
    while (mStateStack.size() > 0)
        pop();
}

void StateManager::update(float dt) {
    uint size = mStateStack.size();
    for (uint i = 0; i < size; ++i)
        mStateStack[i]->update(dt);
}

void StateManager::preRender() {
    for (uint i = 0; i < mStateStack.size(); ++i)
        mStateStack[i]->preRender();
}

int StateManager::getTop() const {
    if (mStateStack.size() > 0)
        return mStateStack.back()->getId();
    else
        return S_NO_STATE;
}

void StateManager::handleEvent(EventDataPtr eventData) {
    // Handle state reloading
    if (eventIs<EvtData_KeyDown>(eventData)) {
        auto castedEventData = castEvent<EvtData_KeyDown>(eventData);
        if (castedEventData->keycode == SDLK_F8) reload();
    }
}

SharedPtr<State> StateManager::getStateById(uint id) {
    auto it = mStateMap.find(id);
    assert(it != mStateMap.end());
    return (*it).second;
}
}
