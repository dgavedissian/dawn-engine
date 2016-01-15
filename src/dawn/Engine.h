/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

NAMESPACE_BEGIN

class EventSystem;
class LuaState;
class Input;
class Renderer;
class UI;
class Audio;
class PhysicsWorld;
class SceneManager;
class StarSystem;
class StateManager;
class Camera;

typedef std::function<void(float)> EngineTickCallback;

// Application Base
class DW_API Engine
{
public:
    Engine(const String& game, const String& version);
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
    const String& GetBasePath() const { return mBasePath; }

    /// Get the preferences path of the application
    /// @returns The pref path
    const String& GetPrefPath() const { return mPrefPath; }

    // TODO: Remove this
    DEPRECATED void SetStarSystem(StarSystem* ss) { mStarSystem = ss; }

	// Engine accessors
	Audio* GetAudio() { return mAudio; }
    EventSystem* GetEventSystem() { return mEventSystem; }
    LuaState* GetLuaState() { return mLuaState; }
    Input* GetInput() { return mInput; }
    Renderer* GetRenderer() { return mRenderer; }
    UI* GetUI() { return mUI; }
    PhysicsWorld* GetPhysicsWorld() { return mPhysicsWorld; }
    SceneManager* GetSceneMgr() { return mSceneMgr; }
    StarSystem* GetStarSystem() { return mStarSystem; }
    StateManager* GetStateMgr() { return mStateMgr; }

private:
    bool mInitialised;
    bool mRunning;
    bool mSaveConfigOnExit;

    String mGameName;
    String mGameVersion;

    // File paths
    String mBasePath;
    String mPrefPath;

    // Filenames
    String mLogFile;
    String mConfigFile;

    // Current main camera
    Camera* mMainCamera;

	// Engine Classes
	Audio* mAudio;
    EventSystem* mEventSystem;
    LuaState* mLuaState;
    Input* mInput;
    Renderer* mRenderer;
    UI* mUI;
    PhysicsWorld* mPhysicsWorld;
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
