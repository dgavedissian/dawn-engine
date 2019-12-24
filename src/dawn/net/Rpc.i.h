/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */

#include "scene/Entity.h"

namespace dw {
template <RpcType Type, typename... Args>
template <typename Component>
RpcSenderImpl<Type, Args...>::RpcSenderImpl(RpcReceiverFuncPtr<Component, Args...> receiver) {
    receiver_ = [receiver](const Entity& entity, const Args&... args) {
        (entity.component<Component>()->*receiver)(args...);
    };
}

template <RpcType Type, typename... Args>
void RpcSenderImpl<Type, Args...>::operator()(const Args&... args) {
    send(args...);
}

template <RpcType Type, typename... Args>
void RpcSenderImpl<Type, Args...>::send(const Args&... args) {
    if (shouldShortCircuit(Type)) {
        assert(entity_);
        receiver_(*entity_, args...);
        return;
    }

    // Pack arguments.
    OutputBitStream bs;
    // Below is a hack to apply stream::write to all args as we can't use C++17 folds. We use
    // the expression {(f(args), 0)...} to generate an array of 0's (as we use the comma
    // operator to discard the result of the function), then ignore the array by casting to
    // void.
    auto t = {(stream::write<Args>(bs, args), 0)...};
    (void)t;

    // Send.
    sendRpcPayload(Type, bs);
}

template <RpcType Type, typename... Args>
void RpcSenderImpl<Type, Args...>::receiveRpcPayload(const Entity& entity,
                                                     const Vector<byte>& payload) {
    InputBitStream bs(payload);
    receiver_(entity, stream::read<Args>(bs)...);
}

template <typename Component, RpcType Type, typename... Args>
RpcBinding BindRpc(RpcSenderMemberPtr<Component, Type, Args...> sender) {
    return [sender](const Entity& e) -> RpcSender& { return e.component<Component>()->*sender; };
}
}  // namespace dw
