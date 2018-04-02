/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "scene/Component.h"
#include "scene/Entity.h"
#include "core/io/InputStream.h"
#include "core/io/OutputStream.h"

#include "net/BitStream.h"
#include "net/NetRole.h"
#include "net/RepProperty.h"
#include "net/Rpc.h"

namespace dw {
// Replication layout.
class DW_API RepLayout {
public:
    RepLayout() = default;
    RepLayout(const RepPropertyList& property_list, const RpcBindingList& rpc_list);

    RepLayout operator+(const RepLayout& other);
    RepLayout& operator+=(const RepLayout& other);

    template <typename... Components> static RepLayout build() {
        RepLayout combined_rep_layout;
        auto a = {(combined_rep_layout += Components::repLayout())...};
        (void)a;
        return combined_rep_layout;
    }

private:
    RepPropertyList property_list_;
    Map<RpcId, SharedPtr<RpcBinding>> rpc_map_;
    RpcId next_rpc_id_;

private:
    void onAddToEntity(Entity& entity);

    friend class NetData;
};

// A component that stores network data such as replication layout and roles.
class DW_API NetData : public Component {
public:
    NetData(RepLayout layout);
    void onAddToEntity(Entity* parent);

    void serialise(OutputStream& out);
    void deserialise(InputStream& in);

    void sendRpc(RpcId rpc_id, RpcType type, const Vector<u8>& payload);
    void receiveRpc(RpcId rpc_id, const Vector<u8>& payload);

    NetRole role() const;
    NetRole remoteRole() const;

private:
    Entity* entity_;
    RepLayout rep_layout_;

    NetRole role_;
    NetRole remote_role_;

    friend class Networking;
};
}  // namespace dw