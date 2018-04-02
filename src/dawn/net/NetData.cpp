/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "net/NetData.h"
#include "net/Networking.h"

namespace dw {
RepLayout::RepLayout(const RepPropertyList& property_list, const RpcBindingList& rpc_list)
    : property_list_(property_list), next_rpc_id_(0) {
    for (auto rpc : rpc_list) {
        rpc_map_[next_rpc_id_++] = rpc;
    }
}

RepLayout RepLayout::operator+(const RepLayout& other) {
    RepLayout result;
    result.property_list_ = property_list_;
    result.rpc_map_ = rpc_map_;
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

NetData::NetData(RepLayout rep_layout)
    : entity_(nullptr),
      rep_layout_(std::move(rep_layout)),
      role_(NetRole::None),
      remote_role_(NetRole::None) {
}

void NetData::onAddToEntity(Entity* parent) {
    entity_ = parent;
    rep_layout_.onAddToEntity(*parent);
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

void NetData::sendRpc(RpcId rpc_id, RpcType type, const Vector<u8>& payload) {
    auto* netsystem = entity_->module<Networking>();
    if (netsystem->isServer()) {
        receiveRpc(rpc_id, payload);
    } else {
        netsystem->sendRpc(entity_->id(), rpc_id, type, payload);
    }
}

void NetData::receiveRpc(RpcId rpc_id, const Vector<u8>& payload) {
    auto rpc_func = rep_layout_.rpc_map_.find(rpc_id);
    if (rpc_func == rep_layout_.rpc_map_.end()) {
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