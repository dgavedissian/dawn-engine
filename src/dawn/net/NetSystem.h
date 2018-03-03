/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "net/Yojimbo.h"

#include "scene/Entity.h"
#include "net/NetData.h"

namespace dw {
// TODO: Move EntityPipeline elsewhere
class DW_API EntityPipeline : public Object {
public:
    DW_OBJECT(EntityPipeline);

    EntityPipeline(Context* ctx) : Object(ctx) {
    }
    virtual ~EntityPipeline() = default;
    virtual u32 getEntityMetadata(const Entity& entity) = 0;
    virtual Entity& createEntityFromMetadata(EntityId entity_id, u32 metadata, NetRole role) = 0;
};

enum class ConnectionState { Disconnected, Connecting, Connected };
enum class NetMode { Server, Client };

class DW_API NetSystem : public Subsystem, public yojimbo::Adapter {
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

    // Send/receive messages.
    void update(float dt);

    // Returns true if connected to a server as a client.
    bool isClient() const;

    // Returns true if running as a server.
    bool isServer() const;

    // Returns true if networking is active.
    bool isConnected() const;

    // Entity pipeline.
    // authoritative_proxy_client indicates a client which should receive the entity with Role =
    // AuthoritativeProxy. -1 for none.
    void replicateEntity(const Entity& entity, int authoritative_proxy_client = -1);
    void setEntityPipeline(UniquePtr<EntityPipeline> entity_pipeline);

    // RPCs.
    void sendSpawnRequest(u32 metadata, std::function<void(Entity&)> callback,
                          bool messaging_proxy = false);
    void sendRpc(EntityId entity_id, RpcId rpc_id, RpcType type, const Vector<u8>& payload);

private:
    bool is_server_;
    double time_;

    ConnectionState client_connection_state_;
    UniquePtr<yojimbo::Client> client_;
    UniquePtr<yojimbo::Server> server_;

    UniquePtr<EntityPipeline> entity_pipeline_;
    HashSet<EntityId> replicated_entities_;

    // Client only.
    u32 spawn_request_id_;
    HashMap<u32, std::function<void(Entity&)>> outgoing_spawn_requests_;
    HashMap<EntityId, u32> pending_entity_spawns_;

    // Server only.

private:
    void sendServerCreateEntity(int clientIndex, const Entity& entity,
                                const OutputBitStream& properties, NetRole role);
    void sendServerPropertyReplication(int clientIndex, const Entity& entity,
                                       const OutputBitStream& properties);

    // Implementation of yojimbo::Adapter.
    yojimbo::MessageFactory* CreateMessageFactory(yojimbo::Allocator& allocator) override;
    void OnServerClientConnected(int clientIndex) override;
    void OnServerClientDisconnected(int clientIndex) override;
};

DEFINE_EMPTY_EVENT(JoinServerEvent);
DEFINE_EVENT(ServerClientConnectedEvent, int, clientIndex);
DEFINE_EVENT(ServerClientDisconnectedEvent, int, clientIndex);
}  // namespace dw
