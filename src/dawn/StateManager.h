/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

NAMESPACE_BEGIN

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
    virtual void Enter() {};

    // Called when the state exits (either on pop or change)
    virtual void Exit() {};

    // Updates this state
    virtual void Update(float dt) {};

    // Called before rendering
    // Use this to blit sprites
    virtual void PreRender() {};

    // Get the ID of this state
    virtual uint GetID() const = 0;

    // Returns the name of this state for debugging purposes
    virtual String GetName() const = 0;
};

// State modality - does it own the screen or is it a pop-up?
enum StateModality
{
    SM_EXCLUSIVE,
    SM_POP_UP
};

// Manages the games states
class DW_API StateManager
{
public:
    StateManager();
    ~StateManager();

    // Registers a new state
    void RegisterState(SharedPtr<State> state);

    // Switches the game to a new state. This will clear all current active pop-up states
    void Switch(int id);

    // Pushes a new pop-up state to the top of the state stack
    void Push(int id);

    // Pops a state from the top of the state stack
    void Pop();

    // Reloads the current top state
    void Reload();

    // Pops all states from the state stack
    void Clear();

    // Update all the visible states
    void Update(float dt);

    // Pre-renders all visible states
    void PreRender();

    // Event handler
    void HandleEvent(EventDataPtr e);

    // Accessors
    int GetTop() const;
    SharedPtr<State> GetStateByID(uint id);
    uint GetDepth() const;

private:
    Map<int, SharedPtr<State>> mStateMap;
    Vector<SharedPtr<State>> mStateStack;
};

NAMESPACE_END
