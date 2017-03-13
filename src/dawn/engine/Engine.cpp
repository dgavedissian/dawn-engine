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
      initialised_(false),
      running_(true),
      save_config_on_exit_(true),
      game_name_(game),
      game_version_(version),
      log_file_("engine.log"),
      config_file_("engine.cfg") {
    // TODO(David): Implement base path (where resources are located) and pref path (where to save
    // settings)
    String basePath = "";
    String prefPath = "";

    // Create context
    context_ = new Context(basePath, prefPath);

    // Initialise logging
    context_->addSubsystem<Logger>(context_);
    // TODO(david): Add a file logger to prefPath + log_file_
    log().info("Starting %s %s", game_name_, game_version_);
#ifdef DW_DEBUG
    log().warn("NOTE: This is a debug build!");
#endif
    printSystemInfo();
}

Engine::~Engine() {
    shutdown();
}

void Engine::setup() {
    assert(!initialised_);

    // Low-level subsystems
    context_->addSubsystem<EventSystem>(context_);
    context_->addSubsystem<FileSystem>(context_);

    // Load configuration
    if (context_->subsystem<FileSystem>()->fileExists(config_file_)) {
        log().info("Loading configuration from %s", config_file_);
        context_->loadConfig(config_file_);
    } else {
        log().info("Configuration does not exist, creating %s", config_file_);
    }

    // Initialise the Lua VM first so bindings can be defined in Constructors
    context_->addSubsystem<LuaState>(context_);
    // TODO(David): bind engine services to lua?

    // Build window title
    String gameTitle(game_name_);
    gameTitle += " ";
    gameTitle += game_version_;
#ifdef DW_DEBUG
    gameTitle += " (debug)";
#endif

    // Create the engine systems
    context_->addSubsystem<Input>(context_);
    context_->addSubsystem<Renderer>(context_);
    // mUI = new UI(mRenderer, mInput, mLuaState);
    // mAudio = new Audio;
    // mPhysicsWorld = new PhysicsWorld(mRenderer);
    // mSceneMgr = new SceneManager(mPhysicsWorld, mRenderer->getSceneMgr());
    // mStarSystem = new StarSystem(mRenderer, mPhysicsWorld);
    context_->addSubsystem<StateManager>(context_);

    // Set input viewport size
    /*
    subsystem<Input>()->setViewportSize(subsystem<Renderer>()->getViewportSize());

    // Enumerate available video modes
    Vector<SDL_DisplayMode> displayModes = subsystem<Renderer>()->getDeviceDisplayModes();
    LOG << "Available video modes:";
    for (auto i = displayModes.begin(); i != displayModes.end(); i++) {
        LOG << "\t" << (*i).w << "x" << (*i).h << "@" << (*i).refresh_rate << "Hz"
            << " - Pixel Format: " << SDL_GetPixelFormatName((*i).format);
    }

    // TODO: move this to UI system
    SDL_StartTextInput();
     */

    // Display startup info
    log().info("Current Working Directory: %s", subsystem<FileSystem>()->getWorkingDir());

    // The engine is now initialised
    initialised_ = true;

    // Register event delegate
    ADD_LISTENER(Engine, EvtData_Exit);
}

void Engine::shutdown() {
    if (!initialised_) {
        return;
    }

    // Save config
    if (save_config_on_exit_) {
        context_->saveConfig(config_file_);
    }

    // Remove subsystems
    context_->removeSubsystem<StateManager>();
    context_->clearSubsystems();

    // The engine is no longer initialised
    initialised_ = false;
}

void Engine::run(EngineTickCallback tickFunc) {
    // TODO(David) stub
    Camera* mMainCamera = nullptr;

    // Start the main loop
    const float dt = 1.0f / 60.0f;
    time::TimePoint previousTime = time::beginTiming();
    double accumulator = 0.0;
    while (running_) {
        // mUI->beginFrame();

        // Update game logic
        while (accumulator >= dt) {
            update(dt);
            tickFunc(dt);
            accumulator -= dt;
        }

        // Render a frame
        preRender(mMainCamera);
        context_->subsystem<Renderer>()->frame();

        // Calculate frameTime
        time::TimePoint currentTime = time::beginTiming();
        accumulator += time::elapsed(previousTime, currentTime);
        previousTime = currentTime;
    }

    // Ensure that all states have been exited so no crashes occur later
    context_->subsystem<StateManager>()->clear();
}

void Engine::printSystemInfo() {
    /*
    LOG << "\tPlatform: " << SDL_GetPlatform();
    LOG << "\tBase Path: " << base_path_;
    LOG << "\tPreferences Path: " << pref_path_;
    // TODO: more system info
     */
}

void Engine::update(float dt) {
    context_->subsystem<EventSystem>()->update(0.02f);
    context_->subsystem<StateManager>()->update(dt);
    context_->subsystem<SceneManager>()->update(dt);
}

void Engine::preRender(Camera* camera) {
    context_->subsystem<SceneManager>()->preRender(camera);
    context_->subsystem<StateManager>()->preRender();
}

void Engine::handleEvent(EventDataPtr eventData) {
    assert(eventIs<EvtData_Exit>(eventData));
    running_ = false;
}
}
