/*
* Dawn Engine
* Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
*/
#include "Common.h"
#include "engine/App.h"
#include "core/Timer.h"
#include "DawnEngine.h"

namespace dw {

Engine::Engine(const String& game, const String& version)
    : Object(nullptr),
      mInitialised(false),
      mRunning(true),
      mSaveConfigOnExit(true),
      mGameName(game),
      mGameVersion(version),
      mLogFile("engine.log"),
      mConfigFile("engine.cfg") {
    // TODO(David): Implement base path (where resources are located) and pref path (where to save
    // settings)
    String basePath = "";
    String prefPath = "";

    // Create context
    mContext = new Context(basePath, prefPath);

    // Initialise logging
    mContext->addSubsystem(new Logger(mContext));
    // TODO(david): Add a file logger to prefPath + mLogFile
    getLog().info("Starting %s %s", mGameName, mGameVersion);
#ifdef DW_DEBUG
    getLog().warn("NOTE: This is a debug build!");
#endif
    printSystemInfo();
}

Engine::~Engine() {
    shutdown();
}

void Engine::setup() {
    assert(!mInitialised);

    // Low-level subsystems
    mContext->addSubsystem(new EventSystem(mContext));
    mContext->addSubsystem(new FileSystem(mContext));

    // Load configuration
    if (mContext->getSubsystem<FileSystem>()->fileExists(mConfigFile)) {
        getLog().info("Loading configuration from %s", mConfigFile);
        mContext->loadConfig(mConfigFile);
    } else {
        getLog().info("Configuration does not exist, creating %s", mConfigFile);
    }

    // Initialise the Lua VM first so bindings can be defined in Constructors
    mContext->addSubsystem(new LuaState(mContext));
    // TODO(David): bind engine services to lua?

    // Build window title
    String gameTitle(mGameName);
    gameTitle += " ";
    gameTitle += mGameVersion;
#ifdef DW_DEBUG
    gameTitle += " (debug)";
#endif

    // Create the engine systems
    mContext->addSubsystem(new Input(mContext));
    mContext->addSubsystem(new Renderer(mContext));
    // mUI = new UI(mRenderer, mInput, mLuaState);
    // mAudio = new Audio;
    // mPhysicsWorld = new PhysicsWorld(mRenderer);
    // mSceneMgr = new SceneManager(mPhysicsWorld, mRenderer->getSceneMgr());
    // mStarSystem = new StarSystem(mRenderer, mPhysicsWorld);
    mContext->addSubsystem(new StateManager(mContext));

    // Set input viewport size
    /*
    getSubsystem<Input>()->setViewportSize(getSubsystem<Renderer>()->getViewportSize());

    // Enumerate available video modes
    Vector<SDL_DisplayMode> displayModes = getSubsystem<Renderer>()->getDeviceDisplayModes();
    LOG << "Available video modes:";
    for (auto i = displayModes.begin(); i != displayModes.end(); i++) {
        LOG << "\t" << (*i).w << "x" << (*i).h << "@" << (*i).refresh_rate << "Hz"
            << " - Pixel Format: " << SDL_GetPixelFormatName((*i).format);
    }

    // TODO: move this to UI system
    SDL_StartTextInput();
     */

    // Display startup info
    getLog().info("Current Working Directory: %s", getSubsystem<FileSystem>()->getWorkingDir());

    // The engine is now initialised
    mInitialised = true;

    // Register event delegate
    ADD_LISTENER(Engine, EvtData_Exit);
}

void Engine::shutdown() {
    if (!mInitialised) {
        return;
    }

    // Save config
    if (mSaveConfigOnExit) {
        mContext->saveConfig(mConfigFile);
    }

    // Remove subsystems
    mContext->removeSubsystem<StateManager>();
    mContext->clearSubsystems();

    // The engine is no longer initialised
    mInitialised = false;
}

void Engine::run(EngineTickCallback tickFunc) {
    // TODO(David) stub
    Camera* mMainCamera = nullptr;

    // Start the main loop
    const float dt = 1.0f / 60.0f;
    time::TimePoint previousTime = time::beginTiming();
    double accumulator = 0.0;
    while (mRunning) {
        // mUI->beginFrame();

        // Update game logic
        while (accumulator >= dt) {
            update(dt);
            tickFunc(dt);
            accumulator -= dt;
        }

        // Render a frame
        preRender(mMainCamera);
        mContext->getSubsystem<Renderer>()->frame();

        // Calculate frameTime
        time::TimePoint currentTime = time::beginTiming();
        accumulator += time::elapsed(previousTime, currentTime);
        previousTime = currentTime;
    }

    // Ensure that all states have been exited so no crashes occur later
    mContext->getSubsystem<StateManager>()->clear();
}

void Engine::printSystemInfo() {
    /*
    LOG << "\tPlatform: " << SDL_GetPlatform();
    LOG << "\tBase Path: " << mBasePath;
    LOG << "\tPreferences Path: " << mPrefPath;
    // TODO: more system info
     */
}

void Engine::update(float dt) {
    mContext->getSubsystem<EventSystem>()->update(0.02f);
    mContext->getSubsystem<StateManager>()->update(dt);
    mContext->getSubsystem<SceneManager>()->update(dt);
}

void Engine::preRender(Camera* camera) {
    mContext->getSubsystem<SceneManager>()->preRender(camera);
    mContext->getSubsystem<StateManager>()->preRender();
}

void Engine::handleEvent(EventDataPtr eventData) {
    assert(eventIs<EvtData_Exit>(eventData));
    mRunning = false;
}
}
