/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#include "Common.h"
#include "Input/Input.h"
#include "StateManager.h"

NAMESPACE_BEGIN

StateManager::StateManager()
{
    ADD_LISTENER(StateManager, EvtData_KeyDown);
}

StateManager::~StateManager()
{
    REMOVE_LISTENER(StateManager, EvtData_KeyDown);

    while (!mStateStack.empty())
        Pop();
    mStateMap.clear();
}

void StateManager::RegisterState(SharedPtr<State> state)
{
    mStateMap[state->GetID()] = state;
}

void StateManager::Change(int id)
{
    if (mStateStack.size() > 0)
        Pop();
    Push(id);
}

void StateManager::Push(int id)
{
    if (id == S_NO_STATE)
        return;

    // TODO: We need some kind of way to "pause" the frame timer
    SharedPtr<State> newState = GetStateByID(id);
    mStateStack.push_back(newState);
    newState->Enter();
    LOG << "Pushed " << newState->GetName();
}

void StateManager::Pop()
{
    if (mStateStack.size() > 0)
    {
        SharedPtr<State> back = mStateStack.back();
        back->Exit();
        mStateStack.pop_back();
        LOG << "Popped " << back->GetName();
    }
    else
    {
        LOG << "WARNING: Trying to pop a state when no states are on the stack";
    }
}

void StateManager::Reload()
{
    int top = GetTop();
    Pop();
    Push(top);
}

void StateManager::Clear()
{
    while (mStateStack.size() > 0)
        Pop();
}

void StateManager::Update(float dt)
{
    uint size = mStateStack.size();
    for (uint i = 0; i < size; ++i)
        mStateStack[i]->Update(dt);
}

void StateManager::PreRender()
{
    for (uint i = 0; i < mStateStack.size(); ++i)
        mStateStack[i]->PreRender();
}

int StateManager::GetTop() const
{
    if (mStateStack.size() > 0)
        return mStateStack.back()->GetID();
    else
        return S_NO_STATE;
}

void StateManager::HandleEvent(EventDataPtr eventData)
{
    // Handle state reloading
    if (EventIs<EvtData_KeyDown>(eventData))
    {
        auto castedEventData = StaticPointerCast<EvtData_KeyDown>(eventData);
        if (castedEventData->keycode == SDLK_F8)
            Reload();
    }
}

SharedPtr<State> StateManager::GetStateByID(uint id)
{
    auto it = mStateMap.find(id);
    assert(it != mStateMap.end());
    return (*it).second;
}

uint StateManager::GetDepth() const
{
    return mStateStack.size();
}

NAMESPACE_END
