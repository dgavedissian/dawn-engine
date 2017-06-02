/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

namespace dw {

class App;

typedef std::function<void(float)> EngineTickCallback;

class DW_API Engine : public Object {
public:
    DW_OBJECT(Engine);

    Engine(const String& game, const String& version);
    ~Engine();

    /// Sets up the engine
    void setup();

    /// Shuts down the engine
    void shutdown();

    /// Run the main loop
    /// @param tickFunc Function to run every time the game logic is updated
    void run(EngineTickCallback tickFunc);

private:
    bool initialised_;
    bool running_;
    bool save_config_on_exit_;

    String game_name_;
    String game_version_;

    // Filenames
    String log_file_;
    String config_file_;

    void printSystemInfo();
    void update(float dt);
    void preRender(Camera* camera);
    void handleEvent(EventDataPtr eventData);
};
}  // namespace dw
