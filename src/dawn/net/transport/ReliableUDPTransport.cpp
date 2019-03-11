/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "net/NetInstance.h"

#include "scene/Entity.h"
#include "scene/SceneManager.h"
#include "net/BitStream.h"
#include "net/CNetData.h"
#include "net/CNetTransform.h"
#include "core/GameSession.h"
#include "ReliableUDPTransport.h"

namespace dw {
namespace {
// Assuming 'bytes' is of type 'Vector<byte>'.
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

struct ToServerMessage : public yojimbo::Message {
    Vector<byte> payload;

    template <typename Stream> bool Serialize(Stream& stream) {
        yojimbo_serialize_byte_array(stream, payload);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

struct ToClientMessage : public yojimbo::Message {
    Vector<byte> payload;

    template <typename Stream> bool Serialize(Stream& stream) {
        yojimbo_serialize_byte_array(stream, payload);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

enum MessageType { MT_ToServer, MT_ToClient, MT_Count };

YOJIMBO_MESSAGE_FACTORY_START(NetMessageFactory, MT_Count);
YOJIMBO_DECLARE_MESSAGE_TYPE(MT_ToServer, ToServerMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE(MT_ToClient, ToClientMessage);
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
        context_->logger->withObjectName("Yojimbo").info(
            "yojimbo: %s", str_buffer.substr(0, str_buffer.size() - 1));
        return count;
    }
};

UniquePtr<YojimboContext> YojimboContext::context_ = nullptr;
int YojimboContext::ref_count_ = 0;

#ifdef DW_MSVC
#pragma warning(pop)
#endif
}  // namespace

YojimboAdapter::YojimboAdapter() {
}

YojimboAdapter::YojimboAdapter(Function<void(ClientId)> client_connected,
                               Function<void(ClientId)> client_disconnected)
    : client_connected_(client_connected), client_disconnected_(client_disconnected) {
}

yojimbo::MessageFactory* YojimboAdapter::CreateMessageFactory(yojimbo::Allocator& allocator) {
    return YOJIMBO_NEW(allocator, NetMessageFactory, allocator);
}

void YojimboAdapter::OnServerClientConnected(int client_index) {
    if (client_connected_) {
        client_connected_(static_cast<ClientId>(client_index));
    }
}

void YojimboAdapter::OnServerClientDisconnected(int client_index) {
    if (client_disconnected_) {
        client_disconnected_(static_cast<ClientId>(client_index));
    }
}

ReliableUDPServer::ReliableUDPServer(Context* ctx, Function<void(ClientId)> client_connected,
                                     Function<void(ClientId)> client_disconnected)
    : Object(ctx),
      adapter_(nullptr),
      server_(nullptr),
      server_connection_state_(ServerConnectionState::NotListening),
      time_(100.0f) {
    YojimboContext::addRef(ctx);
    adapter_ = makeUnique<YojimboAdapter>(client_connected, client_disconnected);
}

ReliableUDPServer::~ReliableUDPServer() {
    disconnect();
    YojimboContext::release();
}

void ReliableUDPServer::listen(const String& host, u16 port, u16 max_connections) {
    if (server_connection_state_ == ServerConnectionState::Listening) {
        disconnect();
    }

    yojimbo::ClientServerConfig config;
    config.timeout = -1;  // Disable timeout.

    uint8_t privateKey[yojimbo::KeyBytes];
    memset(privateKey, 0, yojimbo::KeyBytes);

    server_ =
        makeUnique<yojimbo::Server>(yojimbo::GetDefaultAllocator(), privateKey,
                                    yojimbo::Address{host.c_str(), port}, config, *adapter_, time_);
    server_->Start(max_connections);
    server_connection_state_ = ServerConnectionState::Listening;
}

void ReliableUDPServer::disconnect() {
    if (server_) {
        assert(server_connection_state_ == ServerConnectionState::Listening);
        server_->Stop();
        server_.reset();
        server_connection_state_ = ServerConnectionState::NotListening;
    }
}

void ReliableUDPServer::update(float dt) {
    server_->AdvanceTime(time_);
    time_ += dt;
    server_->SendPackets();
    server_->ReceivePackets();
}

void ReliableUDPServer::send(ClientId client, const byte* data, u32 length) {
    auto* to_client_message =
        static_cast<ToClientMessage*>(server_->CreateMessage(client, MT_ToClient));
    to_client_message->payload.assign(data, data + length);
    server_->SendMessage(client, 0, to_client_message);
}

Option<ServerPacket> ReliableUDPServer::receive(ClientId client) {
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

bool ReliableUDPServer::isClientConnected(ClientId client) const {
    return server_->IsClientConnected(client);
}

usize ReliableUDPServer::numConnections() const {
    return server_->GetNumConnectedClients();
}

ServerConnectionState ReliableUDPServer::connectionState() const {
    return server_connection_state_;
}

usize ReliableUDPServer::maxConnections() const {
    return server_->GetMaxClients();
}

ReliableUDPClient::ReliableUDPClient(Context* ctx, Function<void()> connected,
                                     Function<void()> connection_failed,
                                     Function<void()> disconnected)
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

ReliableUDPClient::~ReliableUDPClient() {
    YojimboContext::release();
}

void ReliableUDPClient::connect(const String& host, u16 port) {
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

void ReliableUDPClient::disconnect() {
    if (client_) {
        assert(client_connection_state_ > ClientConnectionState::Disconnected);
        client_->Disconnect();
        client_.reset();
        client_connection_state_ = ClientConnectionState::Disconnected;
    }
}

void ReliableUDPClient::update(float dt) {
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

void ReliableUDPClient::send(const byte* data, u32 length) {
    auto* to_server_message = static_cast<ToServerMessage*>(client_->CreateMessage(MT_ToServer));
    to_server_message->payload.assign(data, data + length);
    client_->SendMessage(0, to_server_message);
}

Option<ClientPacket> ReliableUDPClient::receive() {
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

ClientConnectionState ReliableUDPClient::connectionState() const {
    return client_connection_state_;
}
}  // namespace dw
