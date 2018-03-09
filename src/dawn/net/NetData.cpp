/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "net/NetData.h"
#include "net/NetSystem.h"

namespace dw {
RepLayout::RepLayout(const RepPropertyList& property_list) : property_list_(property_list)
{
}

RepLayout& RepLayout::operator+(const RepLayout& other)
{
	property_list_.insert(property_list_.end(), other.property_list_.begin(), other.property_list_.end());
	return *this;
}

RepLayout& RepLayout::operator+=(const RepLayout & other)
{
	property_list_.insert(property_list_.end(), other.property_list_.begin(), other.property_list_.end());
	return *this;
}

NetData::NetData(RepLayout rep_layout)
    : entity_(nullptr),
      rep_layout_(std::move(rep_layout)),
      rpc_allocator_(0),
      role_(NetRole::None),
      remote_role_(NetRole::None) {
}

void NetData::onAddToEntity(Entity* parent) {
    entity_ = parent;
    for (auto& prop : rep_layout_.property_list_) {
        prop->onAddToEntity(*parent);
    }
}

void NetData::serialise(OutputStream& out) {
    for (auto& prop : rep_layout_.property_list_) {
        prop->serialise(out);
    }
}

void NetData::deserialise(InputStream& in) {
    for (auto& prop : rep_layout_.property_list_) {
        prop->deserialise(in);
    }
}

void NetData::registerClientRpc(SharedPtr<RpcBinding> rpc) {
    assert(entity_);
    rpc_list_[rpc_allocator_] = rpc;
    rpc->onAddToEntity(*entity_, rpc_allocator_);
    rpc_allocator_++;
}

void NetData::sendRpc(RpcId rpc_id, RpcType type, const Vector<u8>& payload) {
    auto* netsystem = entity_->subsystem<NetSystem>();
    if (netsystem->isServer()) {
        receiveRpc(rpc_id, payload);
    } else {
        netsystem->sendRpc(entity_->id(), rpc_id, type, payload);
    }
}

void NetData::receiveRpc(RpcId rpc_id, const Vector<u8>& payload) {
    auto rpc_func = rpc_list_.find(rpc_id);
    if (rpc_func == rpc_list_.end()) {
        entity_->log().warn("Received unregistered RPC with ID %s, ignoring.", rpc_id);
    }
    (*rpc_func).second->receiveRpc(payload);
}

NetRole NetData::role() const {
    return role_;
}

NetRole NetData::remoteRole() const {
    return remote_role_;
}
}  // namespace dw