/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "net/NetInstance.h"

#include "scene/Entity.h"
#include "scene/SceneManager.h"
#include "core/GameSession.h"
#include "InProcessTransport.h"

namespace dw {
Map<u16, InProcessServer*> InProcessServer::listening_connections;

InProcessServer::InProcessServer(Context* ctx, Function<void(ClientId)> client_connected,
                                 Function<void(ClientId)> client_disconnected)
    : Object(ctx), server_connection_state_(ServerConnectionState::NotListening), time_(100.0f) {
}

InProcessServer::~InProcessServer() {
    disconnect();
}

void InProcessServer::listen(const String&, u16 port, u16 max_connections) {
    if (server_connection_state_ == ServerConnectionState::Listening) {
        disconnect();
    }

    if (listening_connections.find(port) != listening_connections.end()) {
        log().error("InProcessServer: Port %s is already bound.", port);
        return;
    }

    client_streams_.resize(max_connections);
    listening_connections.emplace(port, this);
    port_ = port;
    server_connection_state_ = ServerConnectionState::Listening;
}

void InProcessServer::disconnect() {
    listening_connections.erase(listening_connections.find(port_));
    client_streams_.clear();
    port_ = 0;
    server_connection_state_ = ServerConnectionState::NotListening;
}

void InProcessServer::update(float dt) {
    time_ += dt;
}

void InProcessServer::send(ClientId client, const byte* data, u32 length) {
    if (client_streams_.size() >= client || client_streams_[client].client == nullptr) {
        log().error("InProcessTransport: Sending packet of size %d to non-existent client %d",
                    length, client);
        return;
    }
    client_streams_[client].outgoing.enqueue(Vector<byte>(data, data + length));
}

Option<ServerPacket> InProcessServer::receive(ClientId client) {
    if (client_streams_.size() >= client || client_streams_[client].client == nullptr) {
        log().error("InProcessTransport: Attempting to receive packets from non-existent client %d",
                    client);
    }

    Vector<byte> data;
    bool has_packet = client_streams_[client].incoming.try_dequeue(data);
    if (!has_packet) {
        return {ServerPacket{client, std::move(data)}};
    } else {
        return {};
    }
}

usize InProcessServer::numConnections() const {
    return client_streams_.size();
}

ServerConnectionState InProcessServer::connectionState() const {
    return server_connection_state_;
}

ClientId InProcessServer::clientConnect(InProcessClient* client) {
    // Find a free client.
    for (ClientId i = 0; i < client_streams_.size(); ++i) {
        if (client_streams_[i].client == nullptr) {
            client_streams_[i].client = client;
            return i;
        }
    }

    // If none was found, the host has too many clients connected.
    return -1;
}

void InProcessServer::clientDisconnect(ClientId id) {
    client_streams_[id] = InProcessDataStream();
}

InProcessDataStream& InProcessServer::clientStream(ClientId id) {
    assert(client_streams_.size() < id);
    return client_streams_[id];
}

InProcessClient::InProcessClient(Context* ctx, Function<void()> connected,
                                 Function<void()> connection_failed, Function<void()> disconnected)
    : Object(ctx),
      client_connection_state_(ClientConnectionState::Disconnected),
      time_(100.0f),
      connected_server_(nullptr),
      connected_(connected),
      connection_failed_(connection_failed),
      disconnected_(disconnected) {
}

InProcessClient::~InProcessClient() {
    disconnect();
}

void InProcessClient::connect(const String&, u16 port) {
    disconnect();

    auto server = InProcessServer::listening_connections.find(port);
    if (server == InProcessServer::listening_connections.end()) {
        log().error("InProcessClient: Failed to connect to port %d. No server is listening.", port);
        return;
    }
    connected_server_ = server->second;
    client_id_ = connected_server_->clientConnect(this);
    if (client_id_ == ClientId(-1)) {
        client_id_ = 0;
        log().error("InProcessClient: Failed to connect to port %d. Server is full.", port);
        client_connection_state_ = ClientConnectionState::Disconnected;
    } else {
        client_connection_state_ = ClientConnectionState::Connected;
    }
}

void InProcessClient::disconnect() {
    if (connected_server_) {
        assert(client_connection_state_ > ClientConnectionState::Disconnected);
        client_connection_state_ = ClientConnectionState::Disconnected;
        connected_server_->clientDisconnect(client_id_);
        connected_server_ = nullptr;
    }
}

void InProcessClient::update(float dt) {
    time_ += dt;
}

void InProcessClient::send(const byte* data, u32 length) {
    connected_server_->clientStream(client_id_).incoming.enqueue(Vector<byte>(data, data + length));
}

Option<ClientPacket> InProcessClient::receive() {
    if (!connected_server_) {
        return {};
    }

    Vector<byte> data;
    bool has_packet = connected_server_->clientStream(client_id_).outgoing.try_dequeue(data);
    if (has_packet) {
        return {ClientPacket{std::move(data)}};
    } else {
        return {};
    }
}

ClientConnectionState InProcessClient::connectionState() const {
    return client_connection_state_;
}
}  // namespace dw
