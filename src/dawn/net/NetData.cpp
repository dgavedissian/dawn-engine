/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "net/NetData.h"
#include "net/NetSystem.h"

namespace dw {
NetData::NetData(ReplicatedPropertyList properties)
    : entity_(nullptr), properties_(std::move(properties)), rpc_allocator_(0) {
}

void NetData::onAddToEntity(Entity* parent) {
    entity_ = parent;
    for (auto& prop : properties_) {
        prop->onAddToEntity(*parent);
    }
}

void NetData::serialise(OutputStream& out) {
    for (auto& prop : properties_) {
        prop->serialise(out);
    }
}

void NetData::deserialise(InputStream& in) {
    for (auto& prop : properties_) {
        prop->deserialise(in);
    }
}

void NetData::registerClientRpc(SharedPtr<RpcHandlerBase> rpc) {
    assert(entity_);
    rpc_list_[rpc_allocator_] = rpc;
    rpc->setRpcId(rpc_allocator_);
    rpc->onAddToEntity(*entity_);
    rpc_allocator_++;
}

void NetData::sendClientRpc(RpcId rpc_id, const Vector<u8>& payload) {
    entity_->subsystem<NetSystem>()->sendClientRpc(entity_->id(), rpc_id, payload);
}

void NetData::receiveClientRpc(RpcId rpc_id, const Vector<u8>& payload) {
    auto rpc_func = rpc_list_.find(rpc_id);
    if (rpc_func == rpc_list_.end()) {
        entity_->log().warn("Received unregistered RPC with ID %s, ignoring.", rpc_id);
    }
    (*rpc_func).second->handle(payload);
}
}  // namespace dw