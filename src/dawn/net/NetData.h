/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "scene/Component.h"
#include "scene/Entity.h"
#include "io/InputStream.h"
#include "io/OutputStream.h"

#include "net/BitStream.h"
#include "net/NetRole.h"
#include "net/RepProperty.h"
#include "net/Rpc.h"

namespace dw {
class DW_API NetData : public Component {
public:
    NetData(RepPropertyList properties);
    void onAddToEntity(Entity* parent);

    void serialise(OutputStream& out);
    void deserialise(InputStream& in);

    void registerClientRpc(SharedPtr<RpcBinding> rpc);
    void sendRpc(RpcId rpc_id, RpcType type, const Vector<u8>& payload);
    void receiveRpc(RpcId rpc_id, const Vector<u8>& payload);

    NetRole role() const;
    NetRole remoteRole() const;

private:
    Entity* entity_;
    RepPropertyList properties_;
    Map<RpcId, SharedPtr<RpcBinding>> rpc_list_;
    RpcId rpc_allocator_;

    NetRole role_;
    NetRole remote_role_;

    friend class NetSystem;
};
}  // namespace dw