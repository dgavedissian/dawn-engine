/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "yojimbo.h"

#include "ecs/Entity.h"

namespace dw {

class DW_API NetSystem : public Object, public yojimbo::Adapter {
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

    // Returns true if connected to a server as a client.
    bool isClient() const;

    // Returns true if running as a server.
    bool isServer() const;

    // Returns true if networking is active.
    bool isConnected() const;

    void replicateEntity(const Entity& entity);

private:
    bool is_server_;
    double time_;
    UniquePtr<yojimbo::Client> client_;
    UniquePtr<yojimbo::Server> server_;

    HashSet<EntityId> replicated_entities_;

    void sendCreateEntity(int clientIndex, const Entity& entity);

    // Implementation of yojimbo::Adapter.
    yojimbo::MessageFactory* CreateMessageFactory(yojimbo::Allocator& allocator) override;
    void OnServerClientConnected(int clientIndex) override;
    void OnServerClientDisconnected(int clientIndex) override;
};
}  // namespace dw
