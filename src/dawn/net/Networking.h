/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "net/Yojimbo.h"

#include "scene/Entity.h"
#include "net/CNetData.h"
#include "net/NetEntityPipeline.h"

namespace dw {
enum class ConnectionState { Disconnected, Connecting, Connected };
enum class NetMode { Server, Client };

class DW_API NetInstance : public Object, public yojimbo::Adapter
{
public:
    DW_OBJECT(NetInstance);

    static UniquePtr<NetInstance> connect(Context* context, const String& host, u16 port);
    static UniquePtr<NetInstance> listen(Context* context, u16 port, u16 max_clients);

    NetInstance(Context* context);
    virtual ~NetInstance();

    // Connect to a server at ip:port.
    void connect(const String& ip, u16 port);

    // Host a server on a port.
    void listen(u16 port, u16 max_clients);

    // Disconnect from a server/stop hosting.
    void disconnect();

    // Send/receive messages.
    void update(float dt);

    // Server update.
    void serverUpdate(float dt);

    // Client update.
    void clientUpdate(float dt);

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
    void setEntityPipeline(UniquePtr<NetEntityPipeline> entity_pipeline);

    // RPCs.
    void sendSpawnRequest(EntityType type, std::function<void(Entity&)> callback,
        bool authoritative_proxy = false);
    void sendRpc(EntityId entity_id, RpcId rpc_id, RpcType type, const Vector<u8>& payload);

private:
    bool is_server_;
    double time_;

    ConnectionState client_connection_state_;
    UniquePtr<yojimbo::Client> client_;
    UniquePtr<yojimbo::Server> server_;

    UniquePtr<NetEntityPipeline> entity_pipeline_;
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
