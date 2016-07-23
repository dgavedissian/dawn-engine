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
    void setup();

    /// Shuts down the engine
    void shutdown();

    /// Run the main loop
    /// @param tickFunc Function to run every time the game logic is updated
    void run(EngineTickCallback tickFunc);

    /// Set the main camera
    /// @param camera Camera
    void setMainCamera(Camera *camera);

    /// Get the main camera
    Camera* getMainCamera() { return mMainCamera; }

    /// Get the base path of the application
    /// @returns The base path
    const String& getBasePath() const { return mBasePath; }

    /// Get the preferences path of the application
    /// @returns The pref path
    const String& getPrefPath() const { return mPrefPath; }

    // TODO: Remove this
    DEPRECATED void setStarSystem(StarSystem* ss) { mStarSystem = ss; }

	// Engine accessors
	Audio* getAudio() { return mAudio; }
    EventSystem* getEventSystem() { return mEventSystem; }
    LuaState* getLuaState() { return mLuaState; }
    Input* getInput() { return mInput; }
    Renderer* getRenderer() { return mRenderer; }
    UI* getUI() { return mUI; }
    PhysicsWorld* getPhysicsWorld() { return mPhysicsWorld; }
    SceneManager* getSceneMgr() { return mSceneMgr; }
    StarSystem* getStarSystem() { return mStarSystem; }
    StateManager* getStateMgr() { return mStateMgr; }

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
    void printSystemInfo();
    void update(float dt, Camera* camera);
    void preRender(Camera* camera);
    void handleEvent(EventDataPtr eventData);
    void bindToLua();

};

NAMESPACE_END
