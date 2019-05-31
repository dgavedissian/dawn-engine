/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#pragma once

#include "CommandLine.h"
#include "ui/UserInterface.h"

#if DW_PLATFORM == DW_WIN32
#include "platform/Windows.h"
#define DW_IMPLEMENT_MAIN(AppClass)                                        \
    int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {                 \
        return dw::Engine::runApp(makeUnique<AppClass>(), __argc, __argv); \
    }
#else
#define DW_IMPLEMENT_MAIN(AppClass)                                    \
    int main(int argc, char** argv) {                                  \
        return dw::Engine::runApp(makeUnique<AppClass>(), argc, argv); \
    }
#endif

namespace dw {
class App;
class GameSession;

class DW_API SessionId {
public:
    SessionId(u32 session_index);

    u32 index() const;

private:
    u32 session_index_;

    friend class Engine;
};

class DW_API Engine : public Object {
public:
    DW_OBJECT(Engine);

    /// Runs an app.
    static int runApp(UniquePtr<App> app, int argc, char** argv);

    Engine(UniquePtr<App> app);
    ~Engine();

    /// Sets up the engine
    void setup(const CommandLine& cmdline, const String& base_name);

    /// Shuts down the engine
    void shutdown();

    /// Run the main loop
    /// @returns Exit code to return to the operating system.
    int run();

    // Add a session.
    SessionId addSession(UniquePtr<GameSession> session);

    // Replace a session.
    void replaceSession(SessionId session_id, UniquePtr<GameSession> session);

    // Remove a session.
    void removeSession(SessionId session_id);

    /// Get the list of command line flags.
    const Set<String>& flags() const;

    /// Get the list of command line arguments.
    const HashMap<String, String>& arguments() const;

private:
    bool initialised_;
    bool running_;
    bool save_config_on_exit_;
    bool headless_;

    UniquePtr<App> app_;

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
    bool loadManifest(const String& manifest_path);

    void onExit(const ExitEvent& data);
};
}  // namespace dw
