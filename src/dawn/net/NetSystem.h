/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "yojimbo.h"

namespace dw {

class DW_API NetSystem : public Object {
public:
    DW_OBJECT(NetSystem);

    NetSystem(Context* context);
    virtual ~NetSystem();

    // Connect to a server at ip:port.
    void connect(const String& ip, u16 port);

    // Host a server on a port.
    void listen(u16 port, u16 max_clients);

    // Disconnect from a server/stop hosting.
    void disconnect();

    void update(float dt);

    bool isServer() const;
    bool isConnected() const;

private:
    bool is_server_;
    double time_;
    UniquePtr<yojimbo::Client> client_;
    UniquePtr<yojimbo::Server> server_;
};
}  // namespace dw
