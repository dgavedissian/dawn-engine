/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Base.h"
#include "net/Rpc.h"
#include "net/NetRole.h"
#include "net/CNetData.h"
#include "net/NetInstance.h"

namespace dw {
RpcSender::RpcSender() : entity_(nullptr), logger_(nullptr), rpc_id_(0) {
}

void RpcSender::onAddToEntity(Entity& entity, RpcId rpc_id) {
    entity_ = &entity;
    logger_ = &entity_->component<CNetData>()->net_->log();
    rpc_id_ = rpc_id;
}

bool RpcSender::shouldShortCircuit(RpcType type) const {
    auto net_data = entity_->component<CNetData>();
    assert(net_data);
    // If we're a client, and trying to send an server RPC (RPC destined for the client).
    if (type == RpcType::Server && net_data->netMode() == NetMode::Client) {
        return true;
    }
    // If we're a server, and trying to send a client RPC.
    if (type == RpcType::Client && net_data->netMode() == NetMode::Server) {
        return true;
    }
    return false;
}

void RpcSender::sendRpcPayload(RpcType type, const OutputBitStream& payload) const {
    auto net_data = entity_->component<CNetData>();
    assert(net_data);
    if (net_data->role() != NetRole::AuthoritativeProxy) {
        logger_->warn("Trying to send a client RPC from a non-authoritative proxy.");
        return;
    }
    net_data->sendRpc(rpc_id_, type, payload.vec_data());
}
}  // namespace dw
