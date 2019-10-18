/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Core.h"
#include "resource/ResourceCache.h"
#include "core/GameSession.h"
#include "renderer/Renderer.h"
#include "script/LuaVM.h"

using namespace dw;

class LuaGameSession : public GameSession {
public:
    DW_OBJECT(LuaGameSession);

    LuaGameSession(Context* ctx, const ResourcePath& lua_script_file, const GameSessionInfo& gsi)
        : GameSession(ctx, gsi), lua_state_(ctx) {
        auto* rc = module<ResourceCache>();

        // Register inputs if a client.
        if (!net_instance_ || net_instance_->netMode() == NetMode::Client) {
            module<Input>()->registerEventSystem(event_system_.get());
        }

        // Create Lua VM and execute script.
        auto script_result = rc->loadRaw(lua_script_file);
        auto execute_result = lua_state_.execute(**script_result);
        if (!execute_result) {
            log().error("Failed to execute %s: %s", lua_script_file, execute_result.error());
        }
    }

    ~LuaGameSession() override {
        if (!net_instance_ || net_instance_->netMode() == NetMode::Client) {
            module<Input>()->unregisterEventSystem(event_system_.get());
        }
    }

    void update(float dt) override {
        GameSession::update(dt);

        module<Renderer>()->rhi()->setViewClear(0, {0.0f, 0.0f, 0.0f, 1.0f});

        // Display FPS information.
        ImGui::SetNextWindowPos({10, 10});
        ImGui::SetNextWindowSize({140, 40});
        if (!ImGui::Begin("FPS", nullptr,
                          ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                              ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings)) {
            ImGui::End();
            return;
        }
        ImGui::Text("FPS:   %f", 1.0f / dt);
        ImGui::Text("Frame: %.4f ms", dt);
        ImGui::End();
    }

private:
    LuaVM lua_state_;
};

class LuaShooter : public App {
public:
    DW_OBJECT(LuaShooter);

    LuaShooter() : App("LuaShooter", DW_VERSION_STR) {
    }

    void init(const CommandLine&) override {
        module<ResourceCache>()->addPath("shooter", "../src/lua_shooter/data");

        engine_->addSession(
            makeUnique<LuaGameSession>(context_, "shooter:script_idea.lua", GameSessionInfo{}));
    }

    void shutdown() override {
    }
};

DW_IMPLEMENT_MAIN(LuaShooter);
