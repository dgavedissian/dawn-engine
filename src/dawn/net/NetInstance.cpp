/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "net/NetInstance.h"

#include "scene/Entity.h"
#include "scene/SceneManager.h"
#include "net/BitStream.h"
#include "net/CNetData.h"
#include "net/CNetTransform.h"
#include "core/GameSession.h"

namespace dw {
namespace {
// Assuming 'bytes' is of type 'Vector<u8>'.
#define yojimbo_serialize_byte_array(stream, bytes)  \
    if (Stream::IsReading) {                         \
        u32 size;                                    \
        serialize_uint32(stream, size);              \
        bytes.resize(size);                          \
        serialize_bytes(stream, bytes.data(), size); \
    } else {                                         \
        auto size = static_cast<u32>(bytes.size());  \
        serialize_uint32(stream, size);              \
        serialize_bytes(stream, bytes.data(), size); \
    }

#ifdef DW_MSVC
#pragma warning(push)
#pragma warning(disable : 4127)
#endif

struct ClientSpawnRequestMessage : public yojimbo::Message {
    u32 request_id;
    EntityType entity_type;
    bool authoritative_proxy;

    template <typename Stream> bool Serialize(Stream& stream) {
        serialize_uint32(stream, request_id);
        serialize_uint32(stream, entity_type);
        serialize_bool(stream, authoritative_proxy);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

struct ClientRpcMessage : public yojimbo::Message {
    EntityId entity_id;
    RpcId rpc_id;
    Vector<u8> payload;

    template <typename Stream> bool Serialize(Stream& stream) {
        serialize_uint64(stream, entity_id);
        auto rpc_id_raw = static_cast<u32>(rpc_id);
        serialize_bits(stream, rpc_id_raw, sizeof(RpcId) * 8);
        rpc_id = static_cast<RpcId>(rpc_id_raw);
        yojimbo_serialize_byte_array(stream, payload);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

struct ServerCreateEntityMessage : public yojimbo::Message {
    EntityId entity_id;
    EntityType entity_type;
    NetRole role;
    Vector<u8> data;

    template <typename Stream> bool Serialize(Stream& stream) {
        serialize_uint64(stream, entity_id);
        serialize_uint32(stream, entity_type);
        auto role_raw = static_cast<u32>(role);
        serialize_bits(stream, role_raw, 8);
        role = static_cast<NetRole>(role_raw);
        yojimbo_serialize_byte_array(stream, data);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

struct ServerPropertyReplicationMessage : public yojimbo::Message {
    EntityId entity_id;
    Vector<u8> data;

    template <typename Stream> bool Serialize(Stream& stream) {
        serialize_uint64(stream, entity_id);
        yojimbo_serialize_byte_array(stream, data);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

struct ServerDestroyEntityMessage : public yojimbo::Message {
    EntityId entity_id;

    template <typename Stream> bool Serialize(Stream& stream) {
        serialize_uint64(stream, entity_id);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

struct ServerSpawnResponseMessage : public yojimbo::Message {
    u32 request_id;
    EntityId entity_id;

    template <typename Stream> bool Serialize(Stream& stream) {
        serialize_uint32(stream, request_id);
        serialize_uint64(stream, entity_id);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

// Naming convention: <Src><Msg> e.g. ServerCreateEntity - Server sending CreateEntity to clients.
enum MessageType {
    MT_ClientSpawnRequest,
    MT_ClientRpc,
    MT_ServerCreateEntity,
    MT_ServerPropertyReplication,
    MT_ServerDestroyEntity,
    MT_ServerSpawnResponse,
    MT_Count
};

YOJIMBO_MESSAGE_FACTORY_START(NetMessageFactory, MT_Count);
YOJIMBO_DECLARE_MESSAGE_TYPE(MT_ClientSpawnRequest, ClientSpawnRequestMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE(MT_ClientRpc, ClientRpcMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE(MT_ServerCreateEntity, ServerCreateEntityMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE(MT_ServerPropertyReplication, ServerPropertyReplicationMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE(MT_ServerDestroyEntity, ServerDestroyEntityMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE(MT_ServerSpawnResponse, ServerSpawnResponseMessage);
YOJIMBO_MESSAGE_FACTORY_FINISH();

class YojimboContext {
public:
    YojimboContext(Logger* logger) : logger(logger) {
        InitializeYojimbo();
        yojimbo_log_level(YOJIMBO_LOG_LEVEL_INFO);
        yojimbo_set_printf_function(PrintFunction);
    }

    ~YojimboContext() {
        ShutdownYojimbo();
    }

    static YojimboContext* addRef(Context* context) {
        if (ref_count_ == 0) {
            context_.reset(new YojimboContext(context->module<Logger>()));
        }
        ref_count_++;
        return context_.get();
    }

    static void release() {
        assert(ref_count_ > 0);
        ref_count_--;
        if (ref_count_ == 0) {
            context_.reset();
        }
    }

private:
    Logger* logger;

    static UniquePtr<YojimboContext> context_;
    static int ref_count_;

    static int PrintFunction(const char* format, ...) {
        assert(context_);

        // Build string buffer.
        va_list args;
        va_start(args, format);
        char buffer[4 * 1024];
        int count = vsnprintf(buffer, 4 * 1024, format, args);
        va_end(args);
        String str_buffer{buffer};

        // Write to logger, trimming the ending '\n' that yojimbo always gives us.
        context_->logger->withObjectName("dw::NetInstance")
            .info("yojimbo: %s", str_buffer.substr(0, str_buffer.size() - 1));
        return count;
    }
};

UniquePtr<YojimboContext> YojimboContext::context_ = nullptr;
int YojimboContext::ref_count_ = 0;

#ifdef DW_MSVC
#pragma warning(pop)
#endif
}  // namespace

UniquePtr<NetInstance> NetInstance::connect(Context* context, GameSession* session,
                                            const String& host, u16 port) {
    auto instance = makeUnique<NetInstance>(context, session);
    instance->connect(host, port);
    return instance;
}

UniquePtr<NetInstance> NetInstance::listen(Context* context, GameSession* session,
                                           const String& host, u16 port, u16 max_clients) {
    auto instance = makeUnique<NetInstance>(context, session);
    instance->listen(host, port, max_clients);
    return instance;
}

NetInstance::NetInstance(Context* ctx, GameSession* session)
    : Object{ctx},
      session_(session),
      is_server_(false),
      time_(100.0f),
      client_connection_state_(ConnectionState::Disconnected),
      client_(nullptr),
      server_(nullptr),
      spawn_request_id_(0) {
    YojimboContext::addRef(ctx);
}

NetInstance::~NetInstance() {
    YojimboContext::release();
}

void NetInstance::connect(const String& ip, u16 port) {
    if (isConnected()) {
        disconnect();
    }

    yojimbo::ClientServerConfig config;
    config.timeout = -1;  // Disable timeout.

    uint8_t privateKey[yojimbo::KeyBytes];
    memset(privateKey, 0, yojimbo::KeyBytes);

    client_ = makeUnique<yojimbo::Client>(yojimbo::GetDefaultAllocator(),
                                          yojimbo::Address{"0.0.0.0"}, config, *this, time_);

    // Decide on client ID.
    u64 clientId = 0;
    yojimbo::random_bytes(reinterpret_cast<uint8_t*>(&clientId), 8);
    log().info("Client id is %ull", clientId);

    // Connect to server.
    client_->InsecureConnect(privateKey, clientId, yojimbo::Address{ip.c_str(), port});
    client_connection_state_ = ConnectionState::Connecting;
    is_server_ = false;
}

void NetInstance::listen(const String& host, u16 port, u16 max_clients) {
    if (isConnected()) {
        disconnect();
    }

    yojimbo::ClientServerConfig config;
    config.timeout = -1;  // Disable timeout.

    uint8_t privateKey[yojimbo::KeyBytes];
    memset(privateKey, 0, yojimbo::KeyBytes);

    server_ =
        makeUnique<yojimbo::Server>(yojimbo::GetDefaultAllocator(), privateKey,
                                    yojimbo::Address{host.c_str(), port}, config, *this, time_);
    server_->Start(max_clients);
    is_server_ = true;
}

void NetInstance::disconnect() {
    if (isServer()) {
        server_->Stop();
        server_.reset();
    } else if (isClient()) {
        client_->Disconnect();
        client_.reset();
        client_connection_state_ = ConnectionState::Disconnected;
    }
}

void NetInstance::update(float dt) {
    // static double time_since_last_replication = 0.0;
    // const double replication_time = 1.0 / 20.0;  // 1 / replication rate in Hz
    time_ += dt;
    if (server_) {
        serverUpdate(dt);
    } else if (client_) {
        clientUpdate(dt);
    }
}

void NetInstance::serverUpdate(float) {
    server_->SendPackets();
    server_->ReceivePackets();

    // Process received messages.
    for (int client_index = 0; client_index < server_->GetNumConnectedClients(); ++client_index) {
        while (true) {
            yojimbo::Message* message = server_->ReceiveMessage(client_index, 0);
            if (!message) {
                break;
            }
            switch (message->GetType()) {
                case MT_ClientSpawnRequest: {
                    auto spawn_message = (ClientSpawnRequestMessage*)message;
                    // Spawn entity using entity type.
                    EntityId entity_id = session_->sceneManager()->reserveEntityId();
                    log().info("Received spawn request, spawning entity (id: %s) with type %s.",
                               entity_id, spawn_message->entity_type);
                    Entity* entity = entity_pipeline_->createEntityFromType(
                        entity_id, spawn_message->entity_type, NetRole::Authority);
                    if (entity) {
                        replicateEntity(*entity,
                                        spawn_message->authoritative_proxy ? client_index : -1);
                    }

                    // Send response.
                    auto response_message = (ServerSpawnResponseMessage*)server_->CreateMessage(
                        client_index, MT_ServerSpawnResponse);
                    if (entity) {
                        response_message->entity_id = entity->id() + 10000;
                    } else {
                        response_message->entity_id = 0;
                    }
                    response_message->request_id = spawn_message->request_id;
                    server_->SendMessage(client_index, 0, response_message);
                    break;
                }
                case MT_ClientRpc: {
                    auto rpc_message = (ClientRpcMessage*)message;
                    EntityId entity_id = rpc_message->entity_id;
                    Entity* entity = session_->sceneManager()->findEntity(entity_id);
                    if (!entity) {
                        log().error("Client RPC: Received from non-existent entity %d", entity_id);
                        break;
                    }
                    auto net_data = entity->component<CNetData>();
                    if (!net_data) {
                        log().error("Client RPC: Entity %d has no CNetData component.", entity_id);
                        break;
                    }
                    net_data->receiveRpc(rpc_message->rpc_id, rpc_message->payload);
                    break;
                }
                default:
                    log().warn("Unexpected message received on server: %d", message->GetType());
            }
        }
    }

    // Send replicated updates.
    for (auto id : replicated_entities_) {
        Entity& entity = *session_->sceneManager()->findEntity(id);
        OutputBitStream properties;
        entity.component<CNetData>()->serialise(properties);
        for (int i = 0; i < server_->GetNumConnectedClients(); ++i) {
            sendServerPropertyReplication(i, entity, properties);
        }
    }

    server_->AdvanceTime(time_);
}

void NetInstance::clientUpdate(float) {
    client_->SendPackets();
    client_->ReceivePackets();

    // Handle Connecting -> Connected transition.
    if (client_connection_state_ == ConnectionState::Connecting && client_->IsConnected()) {
        client_connection_state_ = ConnectionState::Connected;
        session_->eventSystem()->triggerEvent<JoinServerEvent>();
    }
    // Handle Connecting -> Disconnected transition.
    if (client_connection_state_ == ConnectionState::Connecting && client_->IsDisconnected()) {
        client_connection_state_ = ConnectionState::Disconnected;
        // TODO: Trigger 'on connection failed' event.
    }
    // Handle Connected -> Disconnected transition.
    if (client_connection_state_ == ConnectionState::Connected && client_->IsDisconnected()) {
        client_connection_state_ = ConnectionState ::Disconnected;
        // TODO: Trigger 'on disconnect' event.
    }
    while (true) {
        yojimbo::Message* message = client_->ReceiveMessage(0);
        if (!message) {
            break;
        }
        switch (message->GetType()) {
            case MT_ServerCreateEntity: {
                // TODO: Create EntitySpawnPipeline and move this code to there.
                auto create_entity_message = (ServerCreateEntityMessage*)message;
                InputBitStream bs(create_entity_message->data);
                EntityId entity_id = create_entity_message->entity_id;
                EntityType entity_type = create_entity_message->entity_type;
                NetRole role = create_entity_message->role;
                if (entity_pipeline_) {
                    Entity* entity =
                        entity_pipeline_->createEntityFromType(entity_id, entity_type, role);
                    if (entity) {
                        assert(entity->hasComponent<CNetData>());
                        entity->component<CNetData>()->deserialise(bs);
                        entity->component<CNetData>()->role_ = role;
                        entity->component<CNetData>()->remote_role_ = NetRole::Authority;
                        log().info("Created replicated entity %d at %d %d %d", entity_id,
                                   entity->transform()->position.x, entity->transform()->position.y,
                                   entity->transform()->position.z);

                        // If any spawn requests are waiting for an entity to be created,
                        // trigger the callback and clear.
                        if (pending_entity_spawns_.count(entity->id()) > 0) {
                            auto it = outgoing_spawn_requests_.find(
                                pending_entity_spawns_.at(entity->id()));
                            if (it != outgoing_spawn_requests_.end()) {
                                it->second(*entity);
                                outgoing_spawn_requests_.erase(it);
                                pending_entity_spawns_.erase(entity->id());
                            } else {
                                log().error(
                                    "Attempting to trigger an spawn request callback which no "
                                    "longer exists. Entity ID: %s, Request ID: %s",
                                    entity->id(), pending_entity_spawns_.at(entity->id()));
                                pending_entity_spawns_.erase(entity->id());
                            }
                        }
                    } else {
                        log().error("Failed to spawn an entity of type %s. %s returned nullptr.",
                                    entity_type, entity_pipeline_->typeName());
                    }
                } else {
                    log().warn("Attempted to replicate entity, but no entity pipeline setup.");
                    break;
                }
                break;
            }
            case MT_ServerPropertyReplication: {
                auto replication_message = (ServerPropertyReplicationMessage*)message;
                InputBitStream bs(replication_message->data);
                EntityId entity_id = replication_message->entity_id;
                Entity* entity = session_->sceneManager()->findEntity(entity_id);
                if (entity) {
                    entity->component<CNetData>()->deserialise(bs);
                } else {
                    log().warn(
                        "Received replication update for entity %s which does not exist on "
                        "this client. Ignoring.",
                        entity_id);
                }
                break;
            }
            case MT_ServerDestroyEntity: {
                break;
            }
            case MT_ServerSpawnResponse: {
                auto spawn_message = (ServerSpawnResponseMessage*)message;
                if (spawn_message->entity_id == 0) {
                    // TODO: error
                    // Failed to spawn entity on the server.
                    log().warn("Failed to spawn entity on the server. Request ID: %s",
                               spawn_message->request_id);
                } else {
                    Entity* entity = session_->sceneManager()->findEntity(spawn_message->entity_id);
                    if (entity) {
                        auto it = outgoing_spawn_requests_.find(spawn_message->request_id);
                        if (it != outgoing_spawn_requests_.end()) {
                            it->second(*entity);
                            outgoing_spawn_requests_.erase(it);
                        } else {
                            log().warn(
                                "Received spawn response for an unknown spawn request. Entity "
                                "ID: "
                                "%s, Request ID: %s",
                                spawn_message->entity_id, spawn_message->request_id);
                        }
                    } else {
                        // Wait for the entity to be created.
                        pending_entity_spawns_[spawn_message->entity_id] =
                            spawn_message->request_id;
                    };
                }
                break;
            }
            default:
                log().warn("Unexpected message received on client: %d", message->GetType());
        }
    }
    client_->AdvanceTime(time_);
}

bool NetInstance::isClient() const {
    return client_ != nullptr;
}

bool NetInstance::isServer() const {
    return server_ != nullptr;
}

bool NetInstance::isConnected() const {
    return isClient() ? client_connection_state_ == ConnectionState::Connected : isServer();
}

void NetInstance::replicateEntity(const Entity& entity, int authoritative_proxy_client) {
    assert(entity.hasComponent<CNetData>());

    if (!server_) {
        // No-op.
        return;
    }

    // Set roles.
    entity.component<CNetData>()->role_ = NetRole::Authority;
    entity.component<CNetData>()->remote_role_ = NetRole::Proxy;

    // Add to replicated entities list.
    if (replicated_entities_.find(entity.id()) == replicated_entities_.end()) {
        replicated_entities_.insert(entity.id());

        // Serialise replicated properties.
        // TODO: Rewrite InputStream/OutputStream to expose an unreal FArchive like interface, which
        // by default will just write the bytes as-is.
        OutputBitStream properties;
        entity.component<CNetData>()->serialise(properties);

        // Send create entity message to clients.
        for (int i = 0; i < server_->GetNumConnectedClients(); ++i) {
            sendServerCreateEntity(
                i, entity, properties,
                i == authoritative_proxy_client ? NetRole::AuthoritativeProxy : NetRole::Proxy);
        }
    }
}

void NetInstance::setEntityPipeline(SharedPtr<NetEntityPipeline> entity_pipeline) {
    entity_pipeline_ = entity_pipeline;
}

void NetInstance::sendSpawnRequest(EntityType type, std::function<void(Entity&)> callback,
                                   bool authoritative_proxy) {
    assert(isClient());
    assert(isConnected());
    outgoing_spawn_requests_[spawn_request_id_] = std::move(callback);
    auto message = (ClientSpawnRequestMessage*)client_->CreateMessage(MT_ClientSpawnRequest);
    message->request_id = spawn_request_id_;
    message->entity_type = type;
    message->authoritative_proxy = authoritative_proxy;
    client_->SendMessage(0, message);
    spawn_request_id_++;
}

void NetInstance::sendRpc(EntityId entity_id, RpcId rpc_id, RpcType type,
                          const Vector<u8>& payload) {
    assert(isConnected());
    if (type == RpcType::Client) {
        assert(isClient());
        auto message = (ClientRpcMessage*)client_->CreateMessage(MT_ClientRpc);
        message->entity_id = entity_id - 10000;
        message->rpc_id = rpc_id;
        message->payload = payload;
        client_->SendMessage(0, message);
    } else {
        assert(isServer());
        log().warn("Server RPCs not implemented.");
    }
}

void NetInstance::sendServerCreateEntity(int clientIndex, const Entity& entity,
                                         const OutputBitStream& properties, NetRole role) {
    assert(isServer());

    auto message =
        (ServerCreateEntityMessage*)server_->CreateMessage(clientIndex, MT_ServerCreateEntity);
    // TODO: Reserve entity ID which the client will have free.
    message->entity_id = entity.id() + 10000;
    message->role = role;
    message->data = properties.data();
    message->entity_type = entity.typeId();
    server_->SendMessage(clientIndex, 0, message);
}

void NetInstance::sendServerPropertyReplication(int clientIndex, const Entity& entity,
                                                const OutputBitStream& properties) {
    assert(isServer());
    auto message = (ServerPropertyReplicationMessage*)server_->CreateMessage(
        clientIndex, MT_ServerPropertyReplication);
    // TODO: Reserve entity ID which the client will have free.
    message->entity_id = entity.id() + 10000;
    message->data = properties.data();
    server_->SendMessage(clientIndex, 0, message);
}

yojimbo::MessageFactory* NetInstance::CreateMessageFactory(yojimbo::Allocator& allocator) {
    return YOJIMBO_NEW(allocator, NetMessageFactory, allocator);
}

void NetInstance::OnServerClientConnected(int clientIndex) {
    log().info("Client ID %s connected.", clientIndex);

    // Send replicated entities to client.
    for (auto entity_id : replicated_entities_) {
        Entity* entity = session_->sceneManager()->findEntity(entity_id);
        if (entity) {
            OutputBitStream properties;
            entity->component<CNetData>()->serialise(properties);
            sendServerCreateEntity(clientIndex, *entity, properties, NetRole::Proxy);
        } else {
            log().error("Replicated Entity ID %s missing from SceneManager", entity_id);
        }
    }

    // Trigger event.
    session_->eventSystem()->triggerEvent<ServerClientConnectedEvent>(clientIndex);
}

void NetInstance::OnServerClientDisconnected(int clientIndex) {
    log().info("Client ID %s disconnected.", clientIndex);

    // Trigger event.
    session_->eventSystem()->triggerEvent<ServerClientDisconnectedEvent>(clientIndex);
}
}  // namespace dw
