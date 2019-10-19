/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "CommandLine.h"

namespace dw {
class Engine;

class DW_API App : public Object {
public:
    App(String game_name, String game_version)
        : Object{nullptr}, engine_{nullptr}, game_name_{game_name}, game_version_{game_version} {
    }
    virtual ~App() = default;

    virtual void init(const CommandLine& cmdline) = 0;
    virtual void shutdown() = 0;

    String gameName() const {
        return game_name_;
    }
    String gameVersion() const {
        return game_version_;
    }

    friend class Engine;

protected:
    Engine* engine_;
    String game_name_;
    String game_version_;
};
}  // namespace dw
