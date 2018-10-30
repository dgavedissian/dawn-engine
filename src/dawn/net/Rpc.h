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

// Rpc binding interface. Used internally by CNetData.
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

// RPC functor base class.
class DW_API RpcSenderBase {
public:
    RpcSenderBase();
    virtual ~RpcSenderBase() = default;

    // Called by RpcBinding when setting up this sender object.
    void initInternal(CNetData* net_data, Logger* logger, RpcId rpc_id);

protected:
    void sendServerRpc(const OutputBitStream& payload);
    void sendClientRpc(const OutputBitStream& payload);

protected:
    CNetData* net_data_;
    Logger* logger_;
    RpcId rpc_id_;
};

// RPC functor object. Used to send RPCs from the client using operator().
template <RpcType Type, typename... Args> class DW_API RpcSender : public RpcSenderBase {
public:
    void operator()(const Args&... args);
};

// RPCs sent from the server.
template <typename... Args> using ServerRpc = RpcSender<RpcType::Server, Args...>;

// RPCs sent from the authoritative client.
template <typename... Args> using ClientRpc = RpcSender<RpcType::Client, Args...>;

// Rpc binder class.
class DW_API Rpc {
public:
    template <typename Component, RpcType Type, typename... Args>
    using RpcFunctorPtr = RpcSender<Type, Args...>(Component::*);
    template <typename Component, typename... Args>
    using RpcHandlerPtr = void (Component::*)(const Args&...);

    template <typename Component, RpcType Type, typename... Args>
    static RpcBindingPtr bind(RpcFunctorPtr<Component, Type, Args...> functor,
                              RpcHandlerPtr<Component, Args...> handler);

private:
    // RPC binding implementation. This associates the CNetData component and RPC id with the sender
    // functor. In addition, it will execute RPC handlers when receiving a payload.
    template <typename Component, RpcType Type, typename... Args>
    class DW_API RpcBindingImpl : public RpcBinding {
    public:
        RpcBindingImpl(RpcFunctorPtr<Component, Type, Args...> functor,
                       RpcHandlerPtr<Component, Args...> handler);

        // RpcBinding interface
        void onAddToEntity(Entity& entity, RpcId rpc_id) override;
        void receiveRpc(const Vector<byte>& payload) override;

    private:
        RpcFunctorPtr<Component, Type, Args...> functor_;
        RpcHandlerPtr<Component, Args...> handler_;
        Component* component_;
    };
};
}  // namespace dw

// Implementation.
#include "Rpc.i.h"
