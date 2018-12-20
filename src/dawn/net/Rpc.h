/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "net/BitStream.h"
#include "scene/Entity.h"

namespace dw {
class CNetData;

using RpcId = u16;
enum class RpcType {
    Server,  // RPCs sent from the server to the authoritative client.
    Client   // RPCs sent from the authoritative client to the server.
};

// An RPC receiver member function pointer.
template <typename Component, typename... Args>
using RpcReceiverFuncPtr = void (Component::*)(const Args&...);

// Type erased RPC sender object.
class DW_API RpcSender {
public:
    RpcSender();
    RpcSender(const RpcSender&) = delete;
    RpcSender(RpcSender&&) = default;
    virtual ~RpcSender() = default;

    // Called when the containing component is added to an entity (and an RPC ID is allocated).
    void onAddToEntity(Entity& entity, RpcId rpc_id);

    // Implemented when type information about the component and args types are known in the
    // RpcSenderImpl template.
    virtual void receiveRpcPayload(const Entity& entity, const Vector<byte>& payload) = 0;

protected:
    bool shouldShortCircuit(RpcType type) const;

    // Helper functions that pass the data along to the CNetData component (which forwards it to the
    // network layer).
    void sendServerRpcPayload(const OutputBitStream& payload);
    void sendClientRpcPayload(const OutputBitStream& payload);

    // Entity that contains this RPC sender.
    Entity* entity_;

private:
    CNetData* net_data_;
    Logger* logger_;
    RpcId rpc_id_;
};

// An RPC binding is a closure which returns a reference to an RPC sender instance inside an entity.
using RpcBinding = Function<RpcSender&(const Entity&)>;
using RpcBindingList = Vector<RpcBinding>;

// RPC functor object. Used to send RPCs from the client using operator().
template <RpcType Type, typename... Args> class DW_API RpcSenderImpl : public RpcSender {
public:
    template <typename Component> RpcSenderImpl(RpcReceiverFuncPtr<Component, Args...> receiver);

    // Send an RPC.
    void send(const Args&... args);
    void operator()(const Args&... args);

    // Receive an RPC.
    void receiveRpcPayload(const Entity& entity, const Vector<byte>& payload) override;

private:
    Function<void(const Entity&, const Args&...)> receiver_;
};

template <typename... Args> using ServerRpc = RpcSenderImpl<RpcType::Server, Args...>;
template <typename... Args> using ClientRpc = RpcSenderImpl<RpcType::Client, Args...>;

template <typename Component, RpcType Type, typename... Args>
using RpcSenderMemberPtr = RpcSenderImpl<Type, Args...>(Component::*);

template <typename Component, RpcType Type, typename... Args>
static RpcBinding BindRpc(RpcSenderMemberPtr<Component, Type, Args...> sender);
}  // namespace dw

// Implementation.
#include "Rpc.i.h"
