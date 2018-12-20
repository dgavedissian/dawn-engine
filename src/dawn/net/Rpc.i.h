/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */

#include "scene/Entity.h"

namespace dw {
template <RpcType Type, typename... Args>
void RpcSender<Type, Args...>::operator()(const Args&... args) {
    send(args...);
}

template<RpcType Type, typename... Args>
void RpcSender<Type, Args...>::send(const Args &... args) {
    // Pack arguments.
    OutputBitStream bs;
    // Below is a hack to apply stream::write to all args as we can't use C++17 folds. We use
    // the expression {(f(args), 0)...} to generate an array of 0's (as we use the comma
    // operator to discard the result of the function), then ignore the array by casting to
    // void.
    auto t = {(stream::write<Args>(bs, args), 0)...};
    (void)t;

    // Send.
    switch (Type) {
        case RpcType::Server:
            sendServerRpcPayload(bs);
            break;
        case RpcType::Client:
            sendClientRpcPayload(bs);
            break;
    }
}

    template <typename Component, RpcType Type, typename... Args>
RpcBindingPtr Rpc::bind(RpcSenderMemberPtr<Component, Type, Args...> sender) {
    return makeShared<Rpc::RpcBindingImpl<Component, Type, Args...>>(functor, handler);
}

template <typename Component, RpcType Type, typename... Args>
Rpc::RpcBindingImpl<Component, Type, Args...>::RpcBindingImpl(
    Rpc::RpcSenderMemberPtr<Component, Type, Args...> functor,
    Rpc::RpcReceiverFuncPtr<Component, Args...> handler)
    : functor_(functor), handler_(handler), component_(nullptr) {
}

template <typename Component, RpcType Type, typename... Args>
void Rpc::RpcBindingImpl<Component, Type, Args...>::onAddToEntity(Entity& entity, RpcId rpc_id) {
    component_ = entity.component<Component>();
    auto& rpc_sender = component_->*functor_;
    rpc_sender.initInternal(entity.component<CNetData>(), &entity.log(), rpc_id);
}

template <typename Component, RpcType Type, typename... Args>
void Rpc::RpcBindingImpl<Component, Type, Args...>::receiveRpc(const Vector<byte>& payload) {
    InputBitStream bs(payload);
    (component_->*handler_)(stream::read<Args>(bs)...);
}

}  // namespace dw