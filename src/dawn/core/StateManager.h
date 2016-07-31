/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

namespace dw {

// Internal game states
enum
{
    S_NO_STATE = -1,
    S_SANDBOX,
    S_USER_ID_BEGIN
};

// Empty game state
class DW_API State
{
public:
    State() {}
    virtual ~State() {}

    // Called when the state is entered
    virtual void enter() {};

    // Called when the state exits (either on pop or change)
    virtual void exit() {};

    // Updates this state
    virtual void update(float dt) {};

    // Called before rendering
    // Use this to blit sprites
    virtual void preRender() {};

    // Get the ID of this state
    virtual uint getId() const = 0;

    // Returns the name of this state for debugging purposes
    virtual String getName() const = 0;
};

// State modality - does it own the screen or is it a pop-up?
enum StateModality
{
    SM_EXCLUSIVE,
    SM_POP_UP
};

// Manages the games states
class DW_API StateManager : public Object
{
public:
    DW_OBJECT(Object)

    StateManager(Context* context);
    ~StateManager();

    // Registers a new state
    void registerState(SharedPtr<State> state);

    // Switches the game to a new state. This will clear all current active pop-up states
    void changeTo(int id);

    // Pushes a new pop-up state to the top of the state stack
    void push(int id);

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

    // Event handler
    void handleEvent(EventDataPtr e);

    // Accessors
    int getTop() const;
    SharedPtr<State> getStateById(uint id);

private:
    Map<int, SharedPtr<State>> mStateMap;
    Vector<SharedPtr<State>> mStateStack;
};

}
