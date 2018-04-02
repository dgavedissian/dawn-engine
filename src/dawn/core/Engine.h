/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

namespace dw {

class App;
class Camera_OLD;

typedef Function<void(float)> EngineTickCallback;
typedef Function<void(float)> EngineRenderCallback;

class DW_API Engine : public Object {
public:
    DW_OBJECT(Engine);

    Engine(const String& game, const String& version);
    ~Engine();

    /// Sets up the engine
    void setup(int argc, char** argv);

    /// Shuts down the engine
    void shutdown();

    /// Run the main loop
    /// @param tick_callback Function to run every time the game logic is updated.
    /// @param render_callback Function to run every time a frame is rendered.
    void run(EngineTickCallback tick_callback, EngineRenderCallback render_callback);

    /// Access the frame time
    double frameTime() const;

private:
    bool initialised_;
    bool running_;
    bool save_config_on_exit_;

    String game_name_;
    String game_version_;

    double frame_time_;

    // Configuration.
    String log_file_;
    String config_file_;

    void printSystemInfo();
    String basePath() const;
    void update(float dt);
    void preRender(Camera_OLD* camera);
    void postRender();

    void onExit(const ExitEvent& data);
};
}  // namespace dw
