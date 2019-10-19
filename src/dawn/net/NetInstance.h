/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "scene/Entity.h"
#include "net/NetMode.h"
#include "net/CNetData.h"
#include "net/NetEntityPipeline.h"
#include "scene/SceneManager.h"

#include "net/transport/Transport.h"

namespace dw {
class GameSession;
enum class NetTransport { ReliableUDP, InProcess };

using RequestId = u64;

class DW_API NetInstance : public Object {
public:
    DW_OBJECT(NetInstance);

    static UniquePtr<NetInstance> connect(Context* context, GameSession* session,
                                          const String& host, u16 port, NetTransport transport = NetTransport::ReliableUDP);
    static UniquePtr<NetInstance> listen(Context* context, GameSession* session, const String& host,
                                         u16 port, u16 max_clients, NetTransport transport = NetTransport::ReliableUDP);

    NetInstance(Context* context, GameSession* session, NetTransport transport);
    virtual ~NetInstance();

    // Connect to a server at ip:port.
    void connect(const String& ip, u16 port);

    // Host a server on a port.
    void listen(const String& host, u16 port, u16 max_clients);

    // Disconnect from a server/stop hosting.
    void disconnect();

    // Send/receive messages.
    void update(float dt);

private:
    // Server update.
    void serverUpdate(float dt);

    // Client update.
    void clientUpdate(float dt);

public:
    // Return current net mode.
    NetMode netMode() const;

    // Returns true if networking is active.
    bool isConnected() const;

    // Entity pipeline.
    // authoritative_proxy_client indicates a client which should receive the entity with Role =
    // AuthoritativeProxy. -1 for none.
    void replicateEntity(const Entity& entity, int authoritative_proxy_client = -1);
    void setEntityPipeline(SharedPtr<NetEntityPipeline> entity_pipeline);

    // RPCs.
    void sendSpawnRequest(EntityType type, std::function<void(Entity&)> callback,
                          bool authoritative_proxy = false);
    void sendRpc(EntityId entity_id, RpcId rpc_id, RpcType type, const Vector<byte>& payload);

private:
    GameSession* session_;
    const NetTransport transport_;

    bool is_server_;
    UniquePtr<TransportClient> client_;
    UniquePtr<TransportServer> server_;

    SharedPtr<NetEntityPipeline> entity_pipeline_;
    HashSet<EntityId> replicated_entities_;

    // Entity ID mapper
    HashMap<EntityId, EntityId> remote_to_local_entity_id_; // mapping from remote -> local
    HashMap<EntityId, EntityId> local_to_remote_entity_id_; // mapping from local -> remote

    // Client only.
    RequestId spawn_request_id_;
    HashMap<RequestId, std::function<void(Entity&)>> outgoing_spawn_requests_;
    HashMap<EntityId, RequestId> pending_entity_spawns_; // mapping from remote entity ID -> request ID

    // Server only.

private:
    void sendServerCreateEntity(ClientId client_id, const Entity& entity,
                                const OutputBitStream& properties, NetRole role);
    void sendServerPropertyReplication(ClientId client_id, const Entity& entity,
                                       const OutputBitStream& properties);

    void onServerClientConnected(ClientId client_id);
    void onServerClientDisconnected(ClientId client_id);
};

DEFINE_EMPTY_EVENT(JoinServerEvent);
DEFINE_EVENT(ServerClientConnectedEvent, int, clientIndex);
DEFINE_EVENT(ServerClientDisconnectedEvent, int, clientIndex);
}  // namespace dw
