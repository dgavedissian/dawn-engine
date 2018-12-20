/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "scene/Component.h"
#include "scene/Entity.h"
#include "core/io/InputStream.h"
#include "core/io/OutputStream.h"

#include "net/BitStream.h"
#include "net/NetRole.h"
#include "net/NetMode.h"
#include "net/RepProperty.h"
#include "net/Rpc.h"

namespace dw {
class NetInstance;

// Replication layout.
class DW_API RepLayout {
public:
    RepLayout();
    RepLayout(const RepPropertyList& property_list, const RpcBindingList& rpc_list);

    RepLayout operator+(const RepLayout& other) const;
    RepLayout& operator+=(const RepLayout& other);

    template <typename... Components> static RepLayout build() {
        RepLayout combined_rep_layout;
        auto a = {(combined_rep_layout += Components::repLayout())...};
        (void)a;
        return combined_rep_layout;
    }

private:
    RepPropertyList property_list_;
    Map<RpcId, RpcBinding> rpc_map_;
    RpcId next_rpc_id_;

private:
    void onAddToEntity(Entity& entity);

    friend class CNetData;
};

// A component that stores network data such as replication layout and roles.
class DW_API CNetData : public Component {
public:
    CNetData(NetInstance* net, RepLayout layout);
    void onAddToEntity(Entity* parent);

    void serialise(OutputStream& out);
    void deserialise(InputStream& in);

    void sendRpc(RpcId rpc_id, RpcType type, const Vector<byte>& payload);
    void receiveRpc(RpcId rpc_id, const Vector<byte>& payload);

    NetRole role() const;
    NetRole remoteRole() const;

    NetMode netMode() const;

private:
    Entity* entity_;
    NetInstance* net_;
    RepLayout rep_layout_;

    NetRole role_;
    NetRole remote_role_;

    friend class NetInstance;
};
}  // namespace dw
