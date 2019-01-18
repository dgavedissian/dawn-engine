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
}  // namespace dw
