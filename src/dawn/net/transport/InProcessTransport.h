/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#pragma once

#include "net/transport/Transport.h"

namespace dw {
class InProcessClient;

struct InProcessDataStream {
    InProcessClient* client;
    ConcurrentQueue<Vector<byte>> incoming;
    ConcurrentQueue<Vector<byte>> outgoing;

    InProcessDataStream() : client(nullptr), incoming(), outgoing() {
    }
    InProcessDataStream(InProcessClient* client) : client(client), incoming(), outgoing() {
    }
};

class InProcessServer : public Object, public TransportServer {
public:
    DW_OBJECT(InProcessServer);

    InProcessServer(Context* ctx, Function<void(ClientId)> client_connected,
                    Function<void(ClientId)> client_disconnected);
    ~InProcessServer();

    void listen(const String& host, u16 port, u16 max_connections) override;
    void disconnect() override;

    void update(float dt) override;
    void send(ClientId client, const byte* data, u32 length) override;
    Option<ServerPacket> receive(ClientId client) override;
    usize numConnections() const override;

    ServerConnectionState connectionState() const override;

private:
    ServerConnectionState server_connection_state_;
    double time_;
    u16 port_;
    Vector<InProcessDataStream> client_streams_;

    // Servers indexed by port number.
    static Map<u16, InProcessServer*> listening_connections;

    ClientId clientConnect(InProcessClient* client);
    void clientDisconnect(ClientId id);
    InProcessDataStream& clientStream(ClientId id);

    friend class InProcessClient;
};

class InProcessClient : public Object, public TransportClient {
public:
    DW_OBJECT(InProcessClient);

    InProcessClient(Context* ctx, Function<void()> connected, Function<void()> connection_failed,
                    Function<void()> disconnected);
    ~InProcessClient();

    void connect(const String& host, u16 port) override;
    void disconnect() override;

    void update(float dt) override;
    void send(const byte* data, u32 length) override;
    Option<ClientPacket> receive() override;

    ClientConnectionState connectionState() const override;

private:
    ClientConnectionState client_connection_state_;
    double time_;
    InProcessServer* connected_server_;
    ClientId client_id_;

    Function<void()> connected_;
    Function<void()> connection_failed_;
    Function<void()> disconnected_;
};
}  // namespace dw
