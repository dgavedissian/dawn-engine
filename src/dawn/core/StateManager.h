/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "input/Input.h"

namespace dw {

// Internal game states
enum { S_NO_STATE = -1, S_SANDBOX, S_USER_ID_BEGIN };

// State ID.
using StateId = uint;

// Empty game state
class DW_API State {
public:
    State() {
    }
    virtual ~State() {
    }

    // Called when the state is entered
    virtual void enter() {
    }

    // Called when the state exits (either on pop or change)
    virtual void exit() {
    }

    // Updates this state
    virtual void update(float) {
    }

    // Called before rendering
    // Use this to blit sprites
    virtual void preRender() {
    }

    // Get the ID of this state
    virtual StateId id() const = 0;

    // Returns the name of this state for debugging purposes
    virtual String name() const = 0;
};

// State modality - does it own the screen or is it a pop-up?
enum class StateModality { SM_EXCLUSIVE, SM_POP_UP };

// Manages the games states
class DW_API StateManager : public Object {
public:
    DW_OBJECT(StateManager)

    StateManager(Context* context);
    ~StateManager();

    // Registers a new state
    void registerState(SharedPtr<State> state);

    // Switches the game to a new state. This will clear all current active pop-up states
    void changeTo(StateId id);

    // Pushes a new pop-up state to the top of the state stack
    void push(StateId id);

    // Pops a state from the top of the state stack
    void pop();

    // Reloads the current top state
    void reload();

    // Pops all states from the state stack
    void clear();

    // Update all the visible states
    void update(float dt);

    // Pre-renders all visible states
    void preRender();

    // Accessors
    StateId top() const;
    SharedPtr<State> stateById(StateId id);

private:
    Map<StateId, SharedPtr<State>> state_map_;
    Vector<SharedPtr<State>> state_stack_;

    void onKeyDown(const EvtData_KeyDown& data);
};
}  // namespace dw
