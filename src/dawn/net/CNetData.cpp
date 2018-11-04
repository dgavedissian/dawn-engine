/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "net/CNetData.h"
#include "net/NetInstance.h"

namespace dw {
RepLayout::RepLayout() : next_rpc_id_(0) {
}

RepLayout::RepLayout(const RepPropertyList& property_list, const RpcBindingList& rpc_list)
    : property_list_(property_list), next_rpc_id_(0) {
    for (auto& rpc : rpc_list) {
        rpc_map_[next_rpc_id_++] = rpc;
    }
}

RepLayout RepLayout::operator+(const RepLayout& other) {
    RepLayout result;
    result.property_list_ = property_list_;
    result.rpc_map_ = rpc_map_;
    result.next_rpc_id_ = next_rpc_id_;
    result += other;
    return result;
}

RepLayout& RepLayout::operator+=(const RepLayout& other) {
    property_list_.insert(property_list_.end(), other.property_list_.begin(),
                          other.property_list_.end());
    for (auto& rpc : other.rpc_map_) {
        rpc_map_[next_rpc_id_++] = rpc.second;
    }
    return *this;
}

void RepLayout::onAddToEntity(Entity& entity) {
    for (auto& prop : property_list_) {
        prop->onAddToEntity(entity);
    }
    for (auto& rpc : rpc_map_) {
        rpc.second->onAddToEntity(entity, rpc.first);
    }
}
CNetData::CNetData(NetInstance* net, RepLayout rep_layout)
    : entity_(nullptr),
      net_(net),
      rep_layout_(std::move(rep_layout)),
      role_(NetRole::None),
      remote_role_(NetRole::None) {
}

void CNetData::onAddToEntity(Entity* parent) {
    entity_ = parent;
    rep_layout_.onAddToEntity(*parent);
}

void CNetData::serialise(OutputStream& out) {
    for (auto& prop : rep_layout_.property_list_) {
        prop->serialise(out);
    }
}

void CNetData::deserialise(InputStream& in) {
    for (auto& prop : rep_layout_.property_list_) {
        prop->deserialise(in);
    }
}

void CNetData::sendRpc(RpcId rpc_id, RpcType type, const Vector<byte>& payload) {
    if (net_->isServer()) {
        receiveRpc(rpc_id, payload);
    } else {
        net_->sendRpc(entity_->id(), rpc_id, type, payload);
    }
}

void CNetData::receiveRpc(RpcId rpc_id, const Vector<byte>& payload) {
    auto rpc_func = rep_layout_.rpc_map_.find(rpc_id);
    if (rpc_func != rep_layout_.rpc_map_.end()) {
        (*rpc_func).second->receiveRpc(payload);
    } else {
        entity_->log().warn("Received unregistered RPC with ID %s, ignoring.", rpc_id);
    }
}

NetRole CNetData::role() const {
    return role_;
}

NetRole CNetData::remoteRole() const {
    return remote_role_;
}
}  // namespace dw
