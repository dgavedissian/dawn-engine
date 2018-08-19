/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "net/transport/Transport.h"
#include "net/Yojimbo.h"

namespace dw {
class YojimboAdapter : public yojimbo::Adapter {
public:
    YojimboAdapter();
    YojimboAdapter(Function<void(ClientId)> client_connected,
                   Function<void(ClientId)> client_disconnected);

    yojimbo::MessageFactory* CreateMessageFactory(yojimbo::Allocator& allocator) override;
    void OnServerClientConnected(int clientIndex) override;
    void OnServerClientDisconnected(int clientIndex) override;

private:
    Function<void(ClientId)> client_connected_;
    Function<void(ClientId)> client_disconnected_;
};

class YojimboServer : public Object, public TransportServer {
public:
    DW_OBJECT(YojimboServer);

    YojimboServer(Context* ctx, Function<void(ClientId)> client_connected,
                  Function<void(ClientId)> client_disconnected);
    ~YojimboServer();

    void listen(const String& host, u16 port, u16 max_connections) override;
    void disconnect() override;

    void update(float dt) override;
    void send(ClientId client, const byte* data, u32 length) override;
    Option<ServerPacket> receive(ClientId client) override;
    int numConnections() const override;

    ServerConnectionState connectionState() const override;

private:
    UniquePtr<YojimboAdapter> adapter_;
    UniquePtr<yojimbo::Server> server_;
    ServerConnectionState server_connection_state_;
    double time_;
};

class YojimboClient : public Object, public TransportClient {
public:
    DW_OBJECT(YojimboClient);

    YojimboClient(Context* ctx, Function<void()> connected, Function<void()> connection_failed,
                  Function<void()> disconnected);
    ~YojimboClient();

    void connect(const String& host, u16 port) override;
    void disconnect() override;

    void update(float dt) override;
    void send(const byte* data, u32 length) override;
    Option<ClientPacket> receive() override;

    ClientConnectionState connectionState() const override;

private:
    UniquePtr<YojimboAdapter> adapter_;
    UniquePtr<yojimbo::Client> client_;
    ClientConnectionState client_connection_state_;
    double time_;

    Function<void()> connected_;
    Function<void()> connection_failed_;
    Function<void()> disconnected_;
};
/*
class DW_API YojimboServer : public Object, public yojimbo::Adapter {
public:
DW_OBJECT(NetInstance);

static UniquePtr<NetInstance> connect(Context* context, GameSession* session,
                                      const String& host, u16 port);
static UniquePtr<NetInstance> listen(Context* context, GameSession* session, const String& host,
                                     u16 port, u16 max_clients);

NetInstance(Context* context, GameSession* session);
virtual ~NetInstance();

// Connect to a server at ip:port.
void connect(const String& ip, u16 port);

// Host a server on a port.
void listen(const String& host, u16 port, u16 max_clients);

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
void setEntityPipeline(SharedPtr<NetEntityPipeline> entity_pipeline);

// RPCs.
void sendSpawnRequest(EntityType type, std::function<void(Entity&)> callback,
                      bool authoritative_proxy = false);
void sendRpc(EntityId entity_id, RpcId rpc_id, RpcType type, const Vector<byte>& payload);

private:
GameSession* session_;

bool is_server_;
double time_;

ConnectionState client_connection_state_;
UniquePtr<yojimbo::Client> client_;
UniquePtr<yojimbo::Server> server_;

SharedPtr<NetEntityPipeline> entity_pipeline_;
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
*/
}  // namespace dw
