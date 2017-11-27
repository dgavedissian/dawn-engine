/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "ecs/Component.h"
#include "ecs/Entity.h"
#include "io/InputStream.h"
#include "io/OutputStream.h"

#include "net/BitStream.h"

namespace dw {

enum class NetRole {
    Proxy = 1,           // An object which receives replicated properties from the server.
    MessagingProxy = 2,  // A proxy which can send client RPCs.
    Authority = 3,       // Authoritative copy (usually on the server).
    None = 4             // No net role.
};

using RpcId = u16;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// REPLICATED PROPERTIES
////////////////////////////////////////////////////////////////////////////////////////////////////
class DW_API ReplicatedPropertyBase {
public:
    virtual ~ReplicatedPropertyBase() = default;
    virtual void onAddToEntity(Entity& entity) = 0;
    virtual void serialise(OutputStream& out) = 0;
    virtual void deserialise(InputStream& in) = 0;
};

using ReplicatedPropertyPtr = SharedPtr<ReplicatedPropertyBase>;
using ReplicatedPropertyList = Vector<ReplicatedPropertyPtr>;

class DW_API ReplicatedProperty {
public:
    template <typename C, typename P> using PropertyMemberPtr = P C::*;
    template <typename C, typename P> using PropertyReferenceFunc = P& (C::*)();
    template <typename C, typename P> using PropertyGetterFunc = P (C::*)();
    template <typename C, typename P> using PropertySetterFunc = void (C::*)(const P&);

    template <typename C, typename P>
    static ReplicatedPropertyPtr bind(PropertyMemberPtr<C, P> member_ptr) {
        return makeShared<ReplicatedProperty_Member<C, P>>(member_ptr);
    }

    template <typename C, typename P>
    static ReplicatedPropertyPtr bind(PropertyReferenceFunc<C, P> reference_func) {
        return makeShared<ReplicatedProperty_ReferenceFunction<C, P>>(reference_func);
    }

    template <typename C, typename P>
    static ReplicatedPropertyPtr bind(PropertyGetterFunc<C, P> getter_func,
                                      PropertySetterFunc<C, P> setter_func) {
        return makeShared<ReplicatedProperty_Accessors<C, P>>(getter_func, setter_func);
    }

private:
    template <typename C>
    class DW_API ReplicatedProperty_Component : public ReplicatedPropertyBase {
    public:
        ReplicatedProperty_Component() : component_(nullptr) {
        }
        virtual ~ReplicatedProperty_Component() = default;

        void onAddToEntity(Entity& entity) override {
            component_ = entity.component<C>();
            assert(component_ != nullptr);
        }

    protected:
        C* component_;
    };

    template <typename C, typename P>
    class DW_API ReplicatedProperty_Member : public ReplicatedProperty_Component<C> {
    public:
        ReplicatedProperty_Member(PropertyMemberPtr<C, P> member_ptr) : member_ptr_(member_ptr) {
        }

        void serialise(OutputStream& out) override {
            stream::write<P>(out, this->component_->*member_ptr_);
        }

        void deserialise(InputStream& in) override {
            this->component_->*member_ptr_ = stream::read<P>(in);
        }

    private:
        PropertyMemberPtr<C, P> member_ptr_;
    };

    template <typename C, typename P>
    class DW_API ReplicatedProperty_ReferenceFunction : public ReplicatedProperty_Component<C> {
    public:
        ReplicatedProperty_ReferenceFunction(PropertyReferenceFunc<C, P> reference_func)
            : reference_func_(reference_func) {
        }

        void serialise(OutputStream& out) override {
            stream::write<P>(out, (this->component_->*reference_func_)());
        }

        void deserialise(InputStream& in) override {
            (this->component_->*reference_func_)() = stream::read<P>(in);
        }

    private:
        PropertyReferenceFunc<C, P> reference_func_;
    };

    template <typename C, typename P>
    class DW_API ReplicatedProperty_Accessors : public ReplicatedProperty_Component<C> {
    public:
        ReplicatedProperty_Accessors(PropertyGetterFunc<C, P> getter,
                                     PropertySetterFunc<C, P> setter)
            : getter_func_(getter), setter_func_(setter) {
        }

        void serialise(OutputStream& out) override {
            stream::write<P>(out, (this->component_->*getter_func_)());
        }

        void deserialise(InputStream& in) override {
            (this->component_->*setter_func_)(stream::read<P>(in));
        }

    private:
        PropertyGetterFunc<C, P> getter_func_;
        PropertySetterFunc<C, P> setter_func_;
    };
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NET DATA
////////////////////////////////////////////////////////////////////////////////////////////////////

class RpcBinding;

class DW_API NetData : public Component {
public:
    NetData(ReplicatedPropertyList properties);
    void onAddToEntity(Entity* parent);

    void serialise(OutputStream& out);
    void deserialise(InputStream& in);

    void registerClientRpc(SharedPtr<RpcBinding> rpc);
    void sendClientRpc(RpcId rpc_id, const Vector<u8>& payload);
    void receiveClientRpc(RpcId rpc_id, const Vector<u8>& payload);

    NetRole getRole() const;
    NetRole getRemoteRole() const;

private:
    Entity* entity_;
    ReplicatedPropertyList properties_;
    Map<RpcId, SharedPtr<RpcBinding>> rpc_list_;
    RpcId rpc_allocator_;

    NetRole role_;
    NetRole remote_role_;

    friend class NetSystem;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// RPCS
////////////////////////////////////////////////////////////////////////////////////////////////////

class DW_API RpcBinding {
public:
    virtual void setRpcId(RpcId rpc_id) = 0;
    virtual void onAddToEntity(Entity& entity) = 0;
    virtual void handle(const Vector<u8>& data) = 0;
};

// RPC sender.
template <typename... Args> class DW_API ClientRpc {
public:
    ClientRpc() : net_data_(nullptr), rpc_id_(0) {
    }

    void operator()(const Args&... args) {
        if (net_data_->getRole() != NetRole::MessagingProxy) {
            logger_->warn("Trying to send a client RPC from a non-messaging proxy.");
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

        // Send RPC.
        assert(net_data_);
        net_data_->sendClientRpc(rpc_id_, bs.data());
    }

    void setNetDataInternal(NetData* net_data, Logger* logger) {
        net_data_ = net_data;
        logger_ = logger;
    }

    void setRpcIdInternal(RpcId rpc_id) {
        rpc_id_ = rpc_id;
    }

private:
    NetData* net_data_;
    Logger* logger_;
    RpcId rpc_id_;
};

class DW_API Rpc {
public:
    template <typename C, typename... Args> using RpcFunctorPtr = ClientRpc<Args...>(C::*);
    template <typename C, typename... Args> using RpcHandlerPtr = void (C::*)(const Args&...);

    template <typename C, typename... Args>
    static SharedPtr<RpcBinding> bind(RpcFunctorPtr<C, Args...> functor,
                                      RpcHandlerPtr<C, Args...> handler) {
        return makeShared<ClientRpcBinding<C, Args...>>(functor, handler);
    }

private:
    // RPC binding and receiver.
    template <typename C, typename... Args> class DW_API ClientRpcBinding : public RpcBinding {
    public:
        ClientRpcBinding(RpcFunctorPtr<C, Args...> functor, RpcHandlerPtr<C, Args...> handler)
            : functor_(functor), handler_(handler), component_(nullptr) {
        }

        void setRpcId(RpcId rpc_id) override {
            (component_->*functor_).setRpcIdInternal(rpc_id);
        }

        void onAddToEntity(Entity& entity) override {
            component_ = entity.component<C>();
            (component_->*functor_).setNetDataInternal(entity.component<NetData>(), &entity.log());
        }

        void handle(const Vector<u8>& payload) override {
            InputBitStream bs(payload);
            (component_->*handler_)(stream::read<Args>(bs)...);
        }

    private:
        RpcFunctorPtr<C, Args...> functor_;
        RpcHandlerPtr<C, Args...> handler_;
        C* component_;
    };
};
}  // namespace dw