/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "core/Option.h"

namespace dw {
enum class ServerConnectionState { NotListening, Listening };
enum class ClientConnectionState { Disconnected, Connecting, Connected };

using ClientId = u16;

struct ServerPacket {
    ClientId client;
    Vector<byte> data;
};

struct ClientPacket {
    Vector<byte> data;
};

class DW_API TransportServer {
public:
    virtual ~TransportServer() = default;

    virtual void listen(const String& host, u16 port, u16 max_connections) = 0;
    virtual void disconnect() = 0;

    virtual void update(float dt) = 0;
    virtual void send(ClientId client, const byte* data, u32 length) = 0;
    virtual Option<ServerPacket> receive(ClientId client) = 0;
    virtual int numConnections() const = 0;

    virtual ServerConnectionState connectionState() const = 0;
};

class DW_API TransportClient {
public:
    virtual ~TransportClient() = default;
    virtual void connect(const String& host, u16 port) = 0;
    virtual void disconnect() = 0;

    virtual void update(float dt) = 0;
    virtual void send(const byte* data, u32 length) = 0;
    virtual Option<ClientPacket> receive() = 0;

    virtual ClientConnectionState connectionState() const = 0;
};
}  // namespace dw
