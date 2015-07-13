/*
    Dawn Engine
    Copyright (c) 2012-2015 David Avedissian (avedissian.david@gmail.com)
    Author: David Avedissian
*/
#include "Common.h"
#include "DawnEngine.h"

NAMESPACE_BEGIN

Engine* gEngine = nullptr;

// Application entry point
Engine::Engine(const string& game, const string& version)
    : mInitialised(false),
      mRunning(true),
      mSaveConfigOnExit(true),
      mGameName(game),
      mGameVersion(version),
      mBasePath(""),
      mPrefPath(""),
      mLogFile("engine.log"),
      mConfigFile("engine.cfg"),
      mMainCamera(nullptr)
{
    gEngine = this;

    // Get the base path
    char* basePath = SDL_GetBasePath();
    mBasePath = basePath;
    SDL_free(basePath);
#if DW_PLATFORM == DW_WIN32
	mBasePath += "..\\";
#else
	mBasePath += "../";
#endif

    // Get the preferences path
    // Having an empty org will leave a double slash in the path so replace with a single slash.
    char* prefPath = SDL_GetPrefPath("", mGameName.c_str());
    mPrefPath = prefPath;
    SDL_free(prefPath);
#if DW_PLATFORM == DW_WIN32
    mPrefPath.replace(mPrefPath.find("\\\\"), 2, "\\");
#else
    mPrefPath.replace(mPrefPath.find("//"), 2, "/");
#endif

    // Change the working directory
#if DW_PLATFORM == DW_WIN32
    SetCurrentDirectoryA(mBasePath.c_str());
#else
    chdir(mBasePath.c_str());
#endif

    // Initialise logging
    new Log(mPrefPath + mLogFile);
    LOG << "Starting " << mGameName << " " << mGameVersion;
    PrintSystemInfo();
}

Engine::~Engine()
{
    Shutdown();
}

void Engine::Setup()
{
    assert(!mInitialised);

    // Set up SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Create EventSystem
    mEventSystem = new EventSystem;

    // Load configuration
    Config::Load(mPrefPath + mConfigFile);

    // Initialise the Lua VM first so bindings can be defined in Constructors
    mLuaState = new LuaState;
    BindToLua();

    // Create the engine systems
    mInputMgr = new InputManager;
    mRenderSystem = new RenderSystem(mBasePath, mPrefPath, mInputMgr, mGameName + " " + mGameVersion);
    mInterfaceMgr = new InterfaceManager(mRenderSystem, mInputMgr, mLuaState);
    mInputMgr->SetViewportSize(mRenderSystem->GetViewportSize());
    mSoundMgr = new SoundManager;
    mPhysicsMgr = new PhysicsManager(mRenderSystem);
    mSceneMgr = new SceneManager(mPhysicsMgr, mRenderSystem->GetSceneMgr());
    mStarSystem = new StarSystem(mRenderSystem, mPhysicsMgr);
    mStateMgr = new StateManager;

    // Enumerate available video modes
    vector<SDL_DisplayMode> displayModes = mRenderSystem->EnumerateDisplayModes();
    LOG << "Available video modes:";
    for (auto i = displayModes.begin(); i != displayModes.end(); i++)
    {
        LOG << "\t" << (*i).w << "x" << (*i).h << "@" << (*i).refresh_rate << "Hz"
        << " - Pixel Format: " << SDL_GetPixelFormatName((*i).format);
    }

    // TODO: move this to UI system
    SDL_StartTextInput();

    // The engine is now initialised
    mInitialised = true;

    // Register event delegate
    ADD_LISTENER(Engine, EvtData_Exit);
}

void Engine::Shutdown()
{
    if (!mInitialised)
        return;

    // Save config
    if (mSaveConfigOnExit)
        Config::Save();

    // Shutdown the engine
    SAFE_DELETE(mStateMgr);
    SAFE_DELETE(mInterfaceMgr);
    SAFE_DELETE(mStarSystem);
    SAFE_DELETE(mSceneMgr);
    SAFE_DELETE(mPhysicsMgr);
    SAFE_DELETE(mSoundMgr);
    SAFE_DELETE(mInputMgr);
    SAFE_DELETE(mRenderSystem);
    SAFE_DELETE(mLuaState);
	
	// Shut down the event system
	mEventSystem = nullptr;
	EventSystem::release();

	// Close the log
    Log::release();

    // Shut down SDL
    SDL_Quit();

    // The engine is no longer initialised
    mInitialised = false;
}

void Engine::Run(EngineTickCallback tickFunc)
{
    // Start the main loop
    const float dt = 1.0f / 60.0f;
    double previousTime = time::Now();
    double accumulator = 0.0;
    while (mRunning)
    {
        mInterfaceMgr->BeginFrame();

        // Update game logic
        while (accumulator >= dt)
        {
            Update(dt, mMainCamera);
            tickFunc(dt);
            accumulator -= dt;
        }

        // Render a frame
        PreRender(mMainCamera);
        mRenderSystem->RenderFrame(mMainCamera);

        // Calculate frameTime
        double currentTime = time::Now();
        accumulator += currentTime - previousTime;
        previousTime = currentTime;
    }

    // Ensure that all states have been exited so no crashes occur later
    mStateMgr->Clear();
    mSceneMgr->ClearEntities();
}

void Engine::SetMainCamera(Camera *camera)
{
    mMainCamera = camera;
}

void Engine::PrintSystemInfo()
{
    LOG << "\tPlatform: " << SDL_GetPlatform();
    LOG << "\tBase Path: " << mBasePath;
    LOG << "\tPreferences Path: " << mPrefPath;
    // TODO: more system info
}

void Engine::Update(float dt, Camera* camera)
{
    mPhysicsMgr->Update(dt, camera);
    mStarSystem->Update(dt);
    mEventSystem->Update((uint64_t)20);
    mSoundMgr->Update(dt, camera);
    mStateMgr->Update(dt);
    mInterfaceMgr->Update(dt);
    mSceneMgr->Update(dt);
}

void Engine::PreRender(Camera* camera)
{
    mStarSystem->PreRender(camera);
    mSceneMgr->PreRender(camera);
    mStateMgr->PreRender();
    mInterfaceMgr->PreRender();
}

void Engine::HandleEvent(EventDataPtr eventData)
{
    assert(EventIs<EvtData_Exit>(eventData));
    mRunning = false;
}

// Lua functions
void Lua_EnterSandbox()
{
    gEngine->GetStateMgr()->Clear();
    gEngine->GetStateMgr()->Push(S_SANDBOX);
}

void Engine::BindToLua()
{
    mLuaState->Bind()
        .addFunction("EnterSandbox", &Lua_EnterSandbox)
        .beginClass<Vec2>("Vec2")
        .addConstructor<void (*)(void)>()
        .addConstructor<void (*)(float, float)>()
        .addData("x", &Vec2::x)
        .addData("y", &Vec2::y)
        .endClass()
        .beginClass<Vec3>("Vec3")
        .addConstructor<void (*)(void)>()
        .addConstructor<void (*)(float, float, float)>()
        .addData("x", &Vec3::x)
        .addData("y", &Vec3::y)
        .addData("z", &Vec3::z)
        .addFunction("add", (Vec3 (Vec3::*)(const Vec3&) const) &Vec3::Add)
        .addFunction("sub", (Vec3 (Vec3::*)(const Vec3&) const) &Vec3::Sub)
        .addFunction("mul", (Vec3 (Vec3::*)(float) const) &Vec3::Mul)
        .addFunction("div", (Vec3 (Vec3::*)(float) const) &Vec3::Div)
        .addFunction("normalise", &Vec3::Normalize)
        .endClass()
        .beginClass<Vec4>("Vec4")
        .addConstructor<void (*)(void)>()
        .addConstructor<void (*)(float, float, float, float)>()
        .addData("x", &Vec4::x)
        .addData("y", &Vec4::y)
        .addData("z", &Vec4::z)
        .addData("w", &Vec4::w)
        .endClass()
        .beginClass<Quat>("Quat")
        .addConstructor<void (*)(void)>()
        .addConstructor<void (*)(Vec3, float)>()
        .addConstructor<void (*)(float, float, float, float)>()
        .addData("x", &Quat::x)
        .addData("y", &Quat::y)
        .addData("z", &Quat::z)
        .addData("w", &Quat::w)
        .endClass()
        .beginClass<Colour>("Colour")
        .addConstructor<void (*)(void)>()
        .addConstructor<void (*)(float, float, float)>()
        .addConstructor<void (*)(float, float, float, float)>()
        .addData("r", &Colour::r)
        .addData("g", &Colour::g)
        .addData("b", &Colour::b)
        .addData("a", &Colour::a)
        .endClass()
        .beginClass<Position>("Position")
        .addConstructor<void (*)(void)>()
        .addConstructor<void (*)(double, double, double)>()
        .addFunction("add", (Position & (Position::*)(const Vec3&)) & Position::operator+=)
        .addFunction("getRelativeToPoint", &Position::GetRelativeToPoint)
        .addFunction("toCameraSpace", &Position::ToCameraSpace)
        .addStaticFunction("fromCameraSpace", &Position::FromCameraSpace)
        .addData("x", &Position::x)
        .addData("y", &Position::y)
        .addData("z", &Position::z)
        .endClass();
}

NAMESPACE_END
