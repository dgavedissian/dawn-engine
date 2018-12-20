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
enum class RpcType { Server, Client };

// Rpc binding interface. Used internally by CNetData to dispatch .
class DW_API RpcBinding {
public:
    virtual ~RpcBinding() = default;
    // Called by CNetData when registering this RPC with a particular entity.
    virtual void onAddToEntity(Entity& entity, RpcId rpc_id) = 0;
    // Called by CNetData when an RPC is received.
    virtual void receiveRpc(const Vector<byte>& data) = 0;
};

// Useful aliases.
using RpcBindingPtr = SharedPtr<RpcBinding>;
using RpcBindingList = Vector<RpcBindingPtr>;

// Type erased RPC sender object.
class DW_API RpcSenderBase {
public:
    RpcSenderBase();
    virtual ~RpcSenderBase() = default;

    // Called by RpcBinding when setting up this sender object.
    void initInternal(CNetData* net_data, Logger* logger, RpcId rpc_id);

    // Implemented when type information about the component and args types are known in the RpcSender template.
    virtual void receiveRpcPayload(const Entity* entity, const Vector<byte>& payload) = 0;

protected:
    // Helper functions that pass the data along to the CNetData component (which forwards it to the network layer).
    void sendServerRpcPayload(const OutputBitStream &payload);
    void sendClientRpcPayload(const OutputBitStream &payload);

protected:
    CNetData* net_data_;
    Logger* logger_;
    RpcId rpc_id_;
};

// RPC functor object. Used to send RPCs from the client using operator().
template <RpcType Type, typename... Args> class DW_API RpcSender : public RpcSenderBase {
public:
    template <typename Component>
    RpcSender(Rpc::RpcReceiverFuncPtr<Component, Args...> receiver) {
        receiver_ = [receiver] (const Entity& entity, const Args&... args) {
            entity.component<Component>()->*receiver(args...);
        };
    }

    // Send an RPC.
    void send(const Args&... args);
    void operator()(const Args&... args);

    // Receive an RPC.
    void receiveRpcPayload(const Entity& entity, const Vector<byte>& payload) override {
        InputBitStream bs(payload);
        receiver_(entity, stream::read<Args>(bs)...);
    }

private:
    Function<void(const Entity&, const Args&...)> receiver_;
};

// RPCs sent from the server.
template <typename... Args> using ServerRpc = RpcSender<RpcType::Server, Args...>;

// RPCs sent from the authoritative client.
template <typename... Args> using ClientRpc = RpcSender<RpcType::Client, Args...>;

// Rpc binder class.
class DW_API Rpc {
public:
    template <typename Component, RpcType Type, typename... Args>
    using RpcSenderMemberPtr = RpcSender<Type, Args...>(Component::*);
    template <typename Component, typename... Args>
    using RpcReceiverFuncPtr = ComponentReceiver<Component, Args...>::ReceiverFunction;

    template <typename Component, RpcType Type, typename... Args>
    static RpcSender<Type, Args...> receiver(RpcReceiverFuncPtr<Component, Args...> receiver) {
        return RpcSender<Type, Args...>(receiver);
    }

    template <typename Component, RpcType Type, typename... Args>
    static RpcBindingPtr bind(RpcSenderMemberPtr<Component, Type, Args...> sender);

private:
    // RPC binding implementation. This associates the CNetData component and RPC id with the sender
    // functor. In addition, it will execute RPC handlers when receiving a payload.
    template <typename Component, RpcType Type, typename... Args>
    class DW_API RpcBindingImpl : public RpcBinding {
    public:
        RpcBindingImpl(RpcSenderMemberPtr<Component, Type, Args...> sender);

        // RpcBinding interface
        void onAddToEntity(Entity& entity, RpcId rpc_id) override;
        void receiveRpc(const Vector<byte>& payload) override;

    private:
        Component* component_;
        RpcSenderMemberPtr<Component, Type, Args...> sender_;
    };
};
}  // namespace dw

// Implementation.
#include "Rpc.i.h"
