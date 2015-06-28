/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

NAMESPACE_BEGIN

class EventSystem;
class LuaState;
class InputManager;
class RenderSystem;
class InterfaceManager;
class SoundManager;
class PhysicsManager;
class SceneManager;
class StarSystem;
class StateManager;
class Camera;

typedef std::function<void(float)> EngineTickCallback;

// Application Base
class DW_API Engine
{
public:
    Engine(const string& game, const string& version);
    ~Engine();

    /// Sets up the engine
    void Setup();

    /// Shuts down the engine
    void Shutdown();

    /// Run the main loop
    /// @param tickFunc Function to run every time the game logic is updated
    void Run(EngineTickCallback tickFunc);

    /// Set the main camera
    /// @param camera Camera
    void SetMainCamera(Camera *camera);

    /// Get the main camera
    Camera* GetMainCamera() { return mMainCamera; }

    /// Get the base path of the application
    /// @returns The base path
    const string& GetBasePath() const { return mBasePath; }

    /// Get the preferences path of the application
    /// @returns The pref path
    const string& GetPrefPath() const { return mPrefPath; }

    // TODO: Remove this
    DEPRECATED void SetStarSystem(StarSystem* ss) { mStarSystem = ss; }

    // Engine accessors
    EventSystem* GetEventSystem() { return mEventSystem; }
    LuaState* GetLuaState() { return mLuaState; }
    InputManager* GetInputMgr() { return mInputMgr; }
    RenderSystem* GetRenderSystem() { return mRenderSystem; }
    InterfaceManager* GetInterfaceMgr() { return mInterfaceMgr; }
    SoundManager* GetSoundMgr() { return mSoundMgr; }
    PhysicsManager* GetPhysicsMgr() { return mPhysicsMgr; }
    SceneManager* GetSceneMgr() { return mSceneMgr; }
    StarSystem* GetStarSystem() { return mStarSystem; }
    StateManager* GetStateMgr() { return mStateMgr; }

private:
    bool mInitialised;
    bool mRunning;
    bool mSaveConfigOnExit;

    string mGameName;
    string mGameVersion;

    // File paths
    string mBasePath;
    string mPrefPath;

    // Filenames
    string mLogFile;
    string mConfigFile;

    // Current main camera
    Camera* mMainCamera;

    // Engine Classes
    EventSystem* mEventSystem;
    LuaState* mLuaState;
    InputManager* mInputMgr;
    RenderSystem* mRenderSystem;
    InterfaceManager* mInterfaceMgr;
    SoundManager* mSoundMgr;
    PhysicsManager* mPhysicsMgr;
    SceneManager* mSceneMgr;
    StarSystem* mStarSystem;
    StateManager* mStateMgr;

private:
    void PrintSystemInfo();
    void Update(float dt, Camera* camera);
    void PreRender(Camera* camera);
    void HandleEvent(EventDataPtr eventData);
    void BindToLua();
};

NAMESPACE_END
