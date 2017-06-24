/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "engine/App.h"
#include "core/Timer.h"
#include "DawnEngine.h"

// Required for getBasePath/getPrefPath.
#if DW_PLATFORM == DW_MACOS
#include <CoreFoundation/CoreFoundation.h>
#define MAX_PATH 256
#endif

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
}

Engine::~Engine() {
    shutdown();
}

void Engine::setup() {
    assert(!initialised_);

    // Create context.
    context_ = new Context(getBasePath(), "");

    // Initialise file system.
    context_->addSubsystem<FileSystem>();

    // Initialise logging.
    context_->addSubsystem<Logger>();
// TODO(david): Add a file logger to prefPath + log_file_
#ifdef DW_DEBUG
    log().warn("NOTE: This is a debug build!");
#endif

    // Update working directory.
    context_->subsystem<FileSystem>()->setWorkingDir(context_->basePath());

    // Print info.
    log().info("Initialising engine");
    printSystemInfo();

    // Build window title.
    String window_title(game_name_);
    window_title += " ";
    window_title += game_version_;
#ifdef DW_DEBUG
    window_title += " (debug)";
#endif

    // Create the window.
    window_ = makeUnique<Window>(context_, 1280, 800, window_title);

    // Low-level subsystems
    context_->addSubsystem<EventSystem>();

    // Load configuration
    if (context_->subsystem<FileSystem>()->fileExists(config_file_)) {
        log().info("Loading configuration from %s", config_file_);
        context_->loadConfig(config_file_);
    } else {
        log().info("Configuration does not exist, creating %s", config_file_);
    }

    // Initialise the Lua VM first so bindings can be defined in Constructors
    context_->addSubsystem<LuaState>();
    // TODO(David): bind engine services to lua?

    // Create the engine subsystems.
    context_->addSubsystem<Input>();
    context_->addSubsystem<Renderer>(window_.get());
    // mUI = new UI(mRenderer, mInput, mLuaState);
    // mAudio = new Audio;
    // mPhysicsWorld = new PhysicsWorld(mRenderer);
    // mSceneMgr = new SceneManager(mPhysicsWorld, mRenderer->getSceneMgr());
    // mStarSystem = new StarSystem(mRenderer, mPhysicsWorld);
    context_->addSubsystem<StateManager>();
    context_->addSubsystem<ResourceCache>();

    // Set up the ECS architecture.
    auto& em = *context_->addSubsystem<EntityManager>();
    auto& sm = *context_->addSubsystem<SystemManager>();
    sm.addSystem<EntityRenderer>();

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
    log().info("Engine initialised. Starting %s %s", game_name_, game_version_);

    // Register event delegate
    ADD_LISTENER(Engine, EvtData_Exit);
}

void Engine::shutdown() {
    if (!initialised_) {
        return;
    }

    // Save config.
    if (save_config_on_exit_) {
        context_->saveConfig(config_file_);
    }

    // Remove subsystems.
    context_->removeSubsystem<StateManager>();
    context_->clearSubsystems();

    // Destroy window.
    window_.reset();

    // The engine is no longer initialised.
    initialised_ = false;
}

void Engine::run(EngineTickCallback tick_callback, EngineRenderCallback render_callback) {
    // TODO(David) stub
    Camera* main_camera = nullptr;

    // Start the main loop.
    const float dt = 1.0f / 60.0f;
    time::TimePoint previous_time = time::beginTiming();
    double accumulator = 0.0;
    while (running_) {
        // mUI->beginFrame();

        // Message pump.
        window_->pollEvents();
        if (window_->shouldClose()) {
            running_ = false;
        }

        // Update game logic.
        while (accumulator >= dt) {
            update(dt);
            tick_callback(dt);
            accumulator -= dt;
        }

        // Render a frame.
        preRender(main_camera);
        render_callback();
        context_->subsystem<SystemManager>()->getSystem<EntityRenderer>()->dispatchRenderTasks();
        context_->subsystem<Renderer>()->frame();

        // Calculate frameTime.
        time::TimePoint current_time = time::beginTiming();
        accumulator += time::elapsed(previous_time, current_time);
        previous_time = current_time;
    }

    // Ensure that all states have been exited so no crashes occur later.
    context_->subsystem<StateManager>()->clear();
}

void Engine::printSystemInfo() {
#if DW_PLATFORM == DW_WIN32
    String platform = "Windows";
#elif DW_PLATFORM == DW_MACOS
    String platform = "macOS";
#elif DW_PLATFORM == DW_LINUX
    String platform = "Linux";
#endif
    log().info("Platform: %s", platform);
    log().info("Base Path: %s", context()->basePath());
    log().info("Pref Path: %s", context()->prefPath());
    // TODO: more system info
}

String Engine::getBasePath() const {
#if DW_PLATFORM == DW_MACOS
    CFBundleRef main_bundle = CFBundleGetMainBundle();
    CFURLRef resources_url = CFBundleCopyResourcesDirectoryURL(main_bundle);
    char path[MAX_PATH];
    if (!CFURLGetFileSystemRepresentation(resources_url, TRUE, (UInt8*)path, MAX_PATH)) {
        // error
    }
    CFRelease(resources_url);
    String str_path{path};
    str_path += "/";

    // For debugging, move from Resources folder to bin.
    str_path += "../../../";
    return str_path;
#else
    return "";
#endif
}

void Engine::update(float dt) {
    log().debug("Frame Time: %f", dt);

    context_->subsystem<EventSystem>()->update(0.02f);
    context_->subsystem<StateManager>()->update(dt);
    context_->subsystem<SceneManager>()->update(dt);

    context_->subsystem<SystemManager>()->update();
}

void Engine::preRender(Camera* camera) {
    context_->subsystem<SceneManager>()->preRender(camera);
    context_->subsystem<StateManager>()->preRender();
}

void Engine::handleEvent(EventDataPtr eventData) {
    assert(eventIs<EvtData_Exit>(eventData));
    running_ = false;
}
}  // namespace dw
