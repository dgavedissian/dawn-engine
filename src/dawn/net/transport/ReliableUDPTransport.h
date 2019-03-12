/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#pragma once

#include "net/transport/Transport.h"
#include "net/transport/Yojimbo.h"

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

class ReliableUDPServer : public Object, public TransportServer {
public:
    DW_OBJECT(ReliableUDPServer);

    ReliableUDPServer(Context* ctx, Function<void(ClientId)> client_connected,
                  Function<void(ClientId)> client_disconnected);
    ~ReliableUDPServer();

    void listen(const String& host, u16 port, u16 max_connections) override;
    void disconnect() override;

    void update(float dt) override;
    void send(ClientId client, const byte* data, u32 length) override;
    Option<ServerPacket> receive(ClientId client) override;
    bool isClientConnected(ClientId client) const override;
    usize numConnections() const override;
    usize maxConnections() const override;

    ServerConnectionState connectionState() const override;

private:
    UniquePtr<YojimboAdapter> adapter_;
    UniquePtr<yojimbo::Server> server_;
    ServerConnectionState server_connection_state_;
    double time_;
};

class ReliableUDPClient : public Object, public TransportClient {
public:
    DW_OBJECT(ReliableUDPClient);

    ReliableUDPClient(Context* ctx, Function<void()> connected, Function<void()> connection_failed,
                  Function<void()> disconnected);
    ~ReliableUDPClient();

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
}  // namespace dw
