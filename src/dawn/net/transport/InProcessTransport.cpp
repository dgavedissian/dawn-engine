/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "net/NetInstance.h"

#include "scene/Entity.h"
#include "scene/SceneManager.h"
#include "core/GameSession.h"
#include "InProcessTransport.h"

namespace dw {
namespace
{
struct InProcessPacket
{
    byte* data;
    usize length;
};

struct InProcessDataStream
{
    Vector<InProcessPacket> packets;
    Vector<byte> storage;
};

struct InProcessConnection
{
    static Map<u16, UniquePtr<InProcessConnection>> endpoints;

    Vector<InProcessDataStream> incoming_data_per_client;
    Vector<InProcessDataStream> outgoing_data_per_client;
};
}

InProcessServer::InProcessServer(Context* ctx, Function<void(ClientId)> client_connected,
                         Function<void(ClientId)> client_disconnected)
    : Object(ctx),
      server_connection_state_(ServerConnectionState::NotListening),
      time_(100.0f) {
}

InProcessServer::~InProcessServer() {
    disconnect();
}

void InProcessServer::listen(const String& host, u16 port, u16 max_connections) {
    if (server_connection_state_ == ServerConnectionState::Listening) {
        disconnect();
    }
    /*
    yojimbo::ClientServerConfig config;
    config.timeout = -1;  // Disable timeout.

    uint8_t privateKey[yojimbo::KeyBytes];
    memset(privateKey, 0, yojimbo::KeyBytes);

    server_ =
        makeUnique<yojimbo::Server>(yojimbo::GetDefaultAllocator(), privateKey,
                                    yojimbo::Address{host.c_str(), port}, config, *adapter_, time_);
    server_->Start(max_connections);
        */
    server_connection_state_ = ServerConnectionState::Listening;
}

void InProcessServer::disconnect() {
}

void InProcessServer::update(float dt) {
    time_ += dt;
}

void InProcessServer::send(ClientId client, const byte* data, u32 length) {
    auto* to_client_message =
        static_cast<ToClientMessage*>(server_->CreateMessage(client, MT_ToClient));
    to_client_message->payload.assign(data, data + length);
    server_->SendMessage(client, 0, to_client_message);
}

Option<ServerPacket> InProcessServer::receive(ClientId client) {
    if (!server_ || server_->GetNumConnectedClients() == 0) {
        return {};
    }

    // Fetch a message from the next client.
    yojimbo::Message* message = server_->ReceiveMessage(client, 0);
    if (!message) {
        return {};
    }
    assert(message->GetType() == MT_ToServer);
    auto* to_server_message = static_cast<ToServerMessage*>(message);

    // Extract the data.
    ServerPacket packet;
    packet.client = client;
    packet.data = std::move(to_server_message->payload);

    return {packet};
}

int InProcessServer::numConnections() const {
    return server_->GetNumConnectedClients();
}

ServerConnectionState InProcessServer::connectionState() const {
    return server_connection_state_;
}

InProcessClient::InProcessClient(Context* ctx, Function<void()> connected,
                             Function<void()> connection_failed, Function<void()> disconnected)
    : Object(ctx),
      adapter_(nullptr),
      client_(nullptr),
      client_connection_state_(ClientConnectionState::Disconnected),
      time_(100.0f),
      connected_(connected),
      connection_failed_(connection_failed),
      disconnected_(disconnected) {
    YojimboContext::addRef(ctx);
    adapter_ = makeUnique<YojimboAdapter>();
}

InProcessClient::~InProcessClient() {
    YojimboContext::release();
}

void InProcessClient::connect(const String& host, u16 port) {
    yojimbo::ClientServerConfig config;
    config.timeout = -1;  // Disable timeout.

    uint8_t privateKey[yojimbo::KeyBytes];
    memset(privateKey, 0, yojimbo::KeyBytes);

    client_ = makeUnique<yojimbo::Client>(yojimbo::GetDefaultAllocator(),
                                          yojimbo::Address{"0.0.0.0"}, config, *adapter_, time_);

    // Decide on client ID.
    u64 clientId = 0;
    yojimbo::random_bytes(reinterpret_cast<uint8_t*>(&clientId), 8);
    log().info("Client id is %ull", clientId);

    // Connect to server.
    client_->InsecureConnect(privateKey, clientId, yojimbo::Address{host.c_str(), port});
    client_connection_state_ = ClientConnectionState::Connecting;
}

void InProcessClient::disconnect() {
    if (client_) {
        assert(client_connection_state_ > ClientConnectionState::Disconnected);
        client_->Disconnect();
        client_.reset();
        client_connection_state_ = ClientConnectionState::Disconnected;
    }
}

void InProcessClient::update(float dt) {
    client_->AdvanceTime(time_);
    time_ += dt;
    client_->SendPackets();
    client_->ReceivePackets();

    // Handle Connecting -> Connected transition.
    if (client_connection_state_ == ClientConnectionState::Connecting && client_->IsConnected()) {
        client_connection_state_ = ClientConnectionState::Connected;
        if (connected_) {
            connected_();
        }
    }

    // Handle Connecting -> Disconnected transition.
    if (client_connection_state_ == ClientConnectionState::Connecting &&
        client_->IsDisconnected()) {
        client_connection_state_ = ClientConnectionState::Disconnected;
        if (connection_failed_) {
            connection_failed_();
        }
    }

    // Handle Connected -> Disconnected transition.
    if (client_connection_state_ == ClientConnectionState::Connected && client_->IsDisconnected()) {
        client_connection_state_ = ClientConnectionState::Disconnected;
        if (disconnected_) {
            disconnected_();
        }
    }
}

void InProcessClient::send(const byte* data, u32 length) {
    auto* to_server_message = static_cast<ToServerMessage*>(client_->CreateMessage(MT_ToServer));
    to_server_message->payload.assign(data, data + length);
    client_->SendMessage(0, to_server_message);
}

Option<ClientPacket> InProcessClient::receive() {
    if (!client_ || !client_->IsConnected()) {
        return {};
    }

    // Fetch a message from the next client.
    yojimbo::Message* message = client_->ReceiveMessage(0);
    if (!message) {
        return {};
    }
    assert(message->GetType() == MT_ToClient);
    auto* to_client_message = static_cast<ToClientMessage*>(message);

    // Extract the data.
    ClientPacket packet;
    packet.data = std::move(to_client_message->payload);

    return {packet};
}

ClientConnectionState InProcessClient::connectionState() const {
    return client_connection_state_;
}
}  // namespace dw
