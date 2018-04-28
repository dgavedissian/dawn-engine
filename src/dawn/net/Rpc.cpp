/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "net/Rpc.h"
#include "net/NetRole.h"
#include "net/NetData.h"

namespace dw {
RpcSenderBase::RpcSenderBase() : net_data_(nullptr), logger_(nullptr), rpc_id_(0) {
}

void RpcSenderBase::initInternal(NetData* net_data, Logger* logger, RpcId rpc_id) {
    net_data_ = net_data;
    logger_ = logger;
    rpc_id_ = rpc_id;
}

void RpcSenderBase::sendServerRpc(const OutputBitStream& payload) {
    assert(net_data_);
    net_data_->sendRpc(rpc_id_, RpcType::Server, payload.data());
}

void RpcSenderBase::sendClientRpc(const OutputBitStream& payload) {
    assert(net_data_);
    if (net_data_->role() != NetRole::AuthoritativeProxy) {
        logger_->warn("Trying to send a client RPC from a non-authoritative proxy.");
    } else {
        net_data_->sendRpc(rpc_id_, RpcType::Client, payload.data());
    }
}
}  // namespace dw