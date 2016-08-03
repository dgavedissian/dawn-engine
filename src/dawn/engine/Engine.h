/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

namespace dw {

class App;

typedef std::function<void(float)> EngineTickCallback;

int runEngine(App* app, int argc, char** argv);

class DW_API Engine : public Object {
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
    void setMainCamera(Camera* camera);

private:
    bool mInitialised;
    bool mRunning;
    bool mSaveConfigOnExit;

    String mGameName;
    String mGameVersion;

    // Filenames
    String mLogFile;
    String mConfigFile;

private:
    void printSystemInfo();
    void update(float dt, Camera* camera);
    void preRender(Camera* camera);
    void handleEvent(EventDataPtr eventData);
    void bindToLua();
};

}
