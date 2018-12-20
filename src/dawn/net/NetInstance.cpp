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
#include "transport/YojimboTransport.h"

#include "protocol_generated.h"
#include "to_server_generated.h"
#include "to_client_generated.h"
#include "NetInstance.h"

namespace dw {
namespace {
Vector<byte> toVector(const flatbuffers::Vector<uint8_t>& v) {
    return Vector<byte>(v.data(), v.data() + v.size());
}
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
      client_(nullptr),
      server_(nullptr),
      spawn_request_id_(0) {
}

NetInstance::~NetInstance() {
    disconnect();
}

void NetInstance::connect(const String& host, u16 port) {
    auto connected = [this]() { session_->eventSystem()->triggerEvent<JoinServerEvent>(); };
    auto connection_failed = []() {};
    auto disconnected = []() {};
    client_ = makeUnique<YojimboClient>(context(), connected, connection_failed, disconnected);
    client_->connect(host, port);
    is_server_ = false;
}

void NetInstance::listen(const String& host, u16 port, u16 max_clients) {
    auto client_connected = [this](ClientId client_id) { onServerClientConnected(client_id); };
    auto client_disconnected = [this](ClientId client_id) {
        onServerClientDisconnected(client_id);
    };
    server_ = makeUnique<YojimboServer>(context(), client_connected, client_disconnected);
    server_->listen(host, port, max_clients);
    is_server_ = true;
}

void NetInstance::disconnect() {
    if (server_ != nullptr) {
        server_.reset();
    } else if (client_ != nullptr) {
        client_.reset();
    }
}

void NetInstance::update(float dt) {
    if (server_) {
        serverUpdate(dt);
    } else if (client_) {
        clientUpdate(dt);
    }
}

void NetInstance::serverUpdate(float dt) {
    server_->update(dt);

    // Process received messages.
    for (ClientId client_id = 0; client_id < server_->numConnections(); ++client_id) {
        while (true) {
            auto message = server_->receive(client_id);
            if (!message.isPresent()) {
                break;
            }
            auto server_message = GetServerMessage(message->data.data());
            switch (server_message->to_server_type()) {
                case ServerMessageData_ServerSpawnRequest: {
                    auto spawn_message = server_message->to_server_as_ServerSpawnRequest();
                    // Spawn entity using entity type.
                    EntityId entity_id = session_->sceneManager()->reserveEntityId();
                    log().info("Received spawn request, spawning entity (id: %s) with type %s.",
                               entity_id, spawn_message->entity_type());
                    Entity* entity = entity_pipeline_->createEntityFromType(
                        entity_id, spawn_message->entity_type(), NetRole::Authority);
                    if (entity) {
                        replicateEntity(*entity,
                                        spawn_message->authoritative_proxy() ? client_id : -1);
                    }

                    // Send response.
                    flatbuffers::FlatBufferBuilder builder(1024);
                    auto response = CreateClientSpawnResponse(builder, spawn_message->request_id(),
                                                              entity ? entity->id() + 10000 : 0);
                    auto response_message = CreateClientMessage(
                        builder, ClientMessageData_ClientSpawnResponse, response.Union());
                    builder.Finish(response_message);
                    server_->send(client_id, builder.GetBufferPointer(), builder.GetSize());
                    break;
                }
                case ServerMessageData_ServerRpc: {
                    auto rpc_message = server_message->to_server_as_ServerRpc();
                    EntityId entity_id = rpc_message->entity_id();
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
                    net_data->receiveRpc(rpc_message->rpc_id(), toVector(*rpc_message->payload()));
                    break;
                }
                default:
                    log().warn("Unexpected message received on server: %d",
                               server_message->to_server_type());
            }
        }
    }

    // Send replicated updates.
    for (auto id : replicated_entities_) {
        Entity& entity = *session_->sceneManager()->findEntity(id);
        OutputBitStream properties;
        entity.component<CNetData>()->serialise(properties);
        for (ClientId i = 0; i < server_->numConnections(); ++i) {
            sendServerPropertyReplication(i, entity, properties);
        }
    }
}

void NetInstance::clientUpdate(float dt) {
    client_->update(dt);

    while (true) {
        auto message = client_->receive();
        if (!message.isPresent()) {
            break;
        }
        auto client_message = GetClientMessage(message->data.data());
        switch (client_message->to_client_type()) {
            case ClientMessageData_ClientCreateEntity: {
                // TODO: Create EntitySpawnPipeline and move this code to there.
                auto* create_entity_message = client_message->to_client_as_ClientCreateEntity();
                InputBitStream bs(create_entity_message->payload()->data(),
                                  create_entity_message->payload()->size());
                EntityId entity_id = create_entity_message->entity_id();
                EntityType entity_type = create_entity_message->entity_type();
                auto role = static_cast<NetRole>(create_entity_message->role());
                if (entity_pipeline_) {
                    Entity* entity =
                        entity_pipeline_->createEntityFromType(entity_id, entity_type, role);
                    if (entity) {
                        assert(entity->hasComponent<CNetData>());
                        entity->component<CNetData>()->deserialise(bs);
                        entity->component<CNetData>()->role_ = role;
                        entity->component<CNetData>()->remote_role_ = NetRole::Authority;
                        if (entity->transform()) {
                            log().info("Created replicated entity %d at %d %d %d.", entity_id,
                                       entity->transform()->position.x,
                                       entity->transform()->position.y,
                                       entity->transform()->position.z);
                        } else {
                            log().info("Created replicated entity %d with no transform.",
                                       entity_id);
                        }

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
            case ClientMessageData_ClientPropertyUpdateMessage: {
                auto* replication_message =
                    client_message->to_client_as_ClientPropertyUpdateMessage();
                InputBitStream bs(replication_message->payload()->data(),
                                  replication_message->payload()->size());
                EntityId entity_id = replication_message->entity_id();
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
            case ClientMessageData_ClientDestroyEntity: {
                break;
            }
            case ClientMessageData_ClientSpawnResponse: {
                auto* spawn_message = client_message->to_client_as_ClientSpawnResponse();
                if (spawn_message->entity_id() == 0) {
                    // TODO: error
                    // Failed to spawn entity on the server.
                    log().warn("Failed to spawn entity on the server. Request ID: %s",
                               spawn_message->request_id());
                } else {
                    Entity* entity =
                        session_->sceneManager()->findEntity(spawn_message->entity_id());
                    if (entity) {
                        auto it = outgoing_spawn_requests_.find(spawn_message->request_id());
                        if (it != outgoing_spawn_requests_.end()) {
                            it->second(*entity);
                            outgoing_spawn_requests_.erase(it);
                        } else {
                            log().warn(
                                "Received spawn response for an unknown spawn request. Entity "
                                "ID: "
                                "%s, Request ID: %s",
                                spawn_message->entity_id(), spawn_message->request_id());
                        }
                    } else {
                        // Wait for the entity to be created.
                        pending_entity_spawns_[spawn_message->entity_id()] =
                            spawn_message->request_id();
                    };
                }
                break;
            }
            default:
                log().warn("Unexpected message received on client: %d",
                           client_message->to_client_type());
        }
    }
}

NetMode NetInstance::netMode() const {
    if (client_ != nullptr) {
        return NetMode::Client;
    } else if (server_ != nullptr) {
        return NetMode::Server;
    } else {
        return NetMode::None;
    }
}

bool NetInstance::isConnected() const {
    return netMode() == NetMode::Client
               ? client_->connectionState() == ClientConnectionState::Connected
               : netMode() == NetMode::Server;
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
        for (ClientId i = 0; i < server_->numConnections(); ++i) {
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
    assert(netMode() == NetMode::Client);
    assert(isConnected());
    outgoing_spawn_requests_[spawn_request_id_] = std::move(callback);

    flatbuffers::FlatBufferBuilder builder(1024);
    auto request_message =
        CreateServerSpawnRequest(builder, spawn_request_id_++, type, authoritative_proxy);
    auto message =
        CreateServerMessage(builder, ServerMessageData_ServerSpawnRequest, request_message.Union());
    builder.Finish(message);
    client_->send(builder.GetBufferPointer(), builder.GetSize());
}

void NetInstance::sendRpc(EntityId entity_id, RpcId rpc_id, RpcType type,
                          const Vector<byte>& payload) {
    assert(isConnected());
    if (type == RpcType::Client) {
        assert(netMode() == NetMode::Client);

        flatbuffers::FlatBufferBuilder builder(1024);
        auto rpc_message =
            CreateServerRpc(builder, entity_id - 10000, rpc_id, builder.CreateVector(payload));
        auto message =
            CreateServerMessage(builder, ServerMessageData_ServerRpc, rpc_message.Union());
        builder.Finish(message);
        client_->send(builder.GetBufferPointer(), builder.GetSize());
    } else {
        assert(netMode() == NetMode::Server);
        log().warn("Server RPCs not implemented.");
    }
}

void NetInstance::sendServerCreateEntity(ClientId client_id, const Entity& entity,
                                         const OutputBitStream& properties, NetRole role) {
    assert(netMode() == NetMode::Server);

    flatbuffers::FlatBufferBuilder builder(1024);
    // TODO: Reserve entity ID which the client will have free.
    auto create_entity_message = CreateClientCreateEntity(
        builder, entity.id() + 10000, entity.typeId(), static_cast<::NetRole>(role),
        builder.CreateVector(properties.data(), properties.length()));
    auto message = CreateClientMessage(builder, ClientMessageData_ClientCreateEntity,
                                       create_entity_message.Union());
    builder.Finish(message);
    server_->send(client_id, builder.GetBufferPointer(), builder.GetSize());
}

void NetInstance::sendServerPropertyReplication(ClientId client_id, const Entity& entity,
                                                const OutputBitStream& properties) {
    assert(netMode() == NetMode::Server);

    flatbuffers::FlatBufferBuilder builder(1024);
    auto property_update_message = CreateClientPropertyUpdateMessage(
        builder, entity.id() + 10000, builder.CreateVector(properties.data(), properties.length()));
    auto message = CreateClientMessage(builder, ClientMessageData_ClientPropertyUpdateMessage,
                                       property_update_message.Union());
    builder.Finish(message);
    server_->send(client_id, builder.GetBufferPointer(), builder.GetSize());
}

void NetInstance::onServerClientConnected(ClientId client_id) {
    log().info("Client ID %s connected.", client_id);

    // Send replicated entities to client.
    for (auto entity_id : replicated_entities_) {
        Entity* entity = session_->sceneManager()->findEntity(entity_id);
        if (entity) {
            OutputBitStream properties;
            entity->component<CNetData>()->serialise(properties);
            sendServerCreateEntity(client_id, *entity, properties, NetRole::Proxy);
        } else {
            log().error("Replicated Entity ID %s missing from SceneManager", entity_id);
        }
    }

    // Trigger event.
    session_->eventSystem()->triggerEvent<ServerClientConnectedEvent>(client_id);
}

void NetInstance::onServerClientDisconnected(ClientId client_id) {
    log().info("Client ID %s disconnected.", client_id);

    // Trigger event.
    session_->eventSystem()->triggerEvent<ServerClientDisconnectedEvent>(client_id);
}

}  // namespace dw
