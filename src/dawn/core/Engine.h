/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "CommandLine.h"
#include "ui/UserInterface.h"

namespace dw {
class App;
class GameSession;

typedef Function<void(float)> EngineTickCallback;
typedef Function<void(float)> EngineRenderCallback;

class DW_API SessionId {
public:
    SessionId(int session_index);

    u32 index() const;

private:
    u32 session_index_;

    friend class Engine;
};

class DW_API Engine : public Object {
public:
    DW_OBJECT(Engine);

    Engine(const String& game, const String& version);
    ~Engine();

    /// Sets up the engine
    void setup(const CommandLine& cmdline);

    /// Shuts down the engine
    void shutdown();

    /// Run the main loop
    /// @param tick_callback Function to run every time the game logic is updated.
    /// @param render_callback Function to run every time a frame is rendered.
    void run(EngineTickCallback tick_callback, EngineRenderCallback render_callback);

    // Add a session.
    SessionId addSession(UniquePtr<GameSession> session);

    // Replace a session.
    void replaceSession(SessionId session_id, UniquePtr<GameSession> session);

    // Remove a session.
    void removeSession(SessionId session_id);

    /// Access the frame time
    double frameTime() const;

    /// Access the frames per second metric.
    int framesPerSecond() const;

    /// Get the list of command line flags.
    const Set<String>& flags() const;

    /// Get the list of command line arguments.
    const HashMap<String, String>& arguments() const;

private:
    bool initialised_;
    bool running_;
    bool save_config_on_exit_;
    bool headless_;

    String game_name_;
    String game_version_;

    double frame_time_;
    int frames_per_second_;
    int frame_counter_;

    // Engine events and UI.
    UniquePtr<EventSystem> event_system_;
    UniquePtr<UserInterface> ui_;

    Vector<UniquePtr<GameSession>> game_sessions_;
    void forEachSession(const Function<void(GameSession*)>& functor);

    // Configuration.
    String log_file_;
    String config_file_;
    CommandLine cmdline_;

    void printSystemInfo();
    String basePath() const;

    void onExit(const ExitEvent& data);
};
}  // namespace dw
