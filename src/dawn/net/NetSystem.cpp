/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "net/NetSystem.h"

#include "ecs/Entity.h"
#include "ecs/EntityManager.h"
#include "net/BitStream.h"
#include "net/NetData.h"

namespace dw {
namespace {
Logger* yojimbo_logger = nullptr;
int yojimbo_printf_function(const char* format, ...) {
    // Build string buffer.
    va_list args;
    va_start(args, format);
    char buffer[4 * 1024];
    int count = vsnprintf(buffer, 4 * 1024, format, args);
    va_end(args);
    String str_buffer{buffer};

    // Write to logger, trimming the ending '\n' that yojimbo always gives us.
    yojimbo_logger->withObjectName("dw::NetSystem")
        .info("yojimbo: %s", str_buffer.substr(0, str_buffer.size() - 1));
    return count;
}

// Test message.
// inline int GetNumBitsForMessage(uint16_t sequence) {
//    static int messageBitsArray[] = {1,  320, 120, 4, 256, 45,  11, 13, 101, 100, 84,
//                                     95, 203, 2,   3, 8,   512, 5,  3,  7,   50};
//    const int modulus = sizeof(messageBitsArray) / sizeof(int);
//    const int index = sequence % modulus;
//    return messageBitsArray[index];
//}
//
// struct TestMessage : public yojimbo::Message {
//    uint16_t sequence;
//
//    TestMessage() {
//        sequence = 0;
//    }
//
//    template <typename Stream> bool Serialize(Stream& stream) {
//        serialize_bits(stream, sequence, 16);
//
//        int numBits = GetNumBitsForMessage(sequence);
//        int numWords = numBits / 32;
//        uint32_t dummy = 0;
//        for (int i = 0; i < numWords; ++i)
//            serialize_bits(stream, dummy, 32);
//        int numRemainderBits = numBits - numWords * 32;
//        if (numRemainderBits > 0)
//            serialize_bits(stream, dummy, numRemainderBits);
//
//        return true;
//    }
//
//    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
//};
struct ServerCreateEntityMessage : public yojimbo::Message {
    EntityId entity_id;
    Vector<u8> data;

    ServerCreateEntityMessage() {
        entity_id = 0;
    }

    template <typename Stream> bool Serialize(Stream& stream) {
        serialize_uint32(stream, entity_id);
        if (Stream::IsReading) {
            u32 size;
            serialize_uint32(stream, size);
            data.resize(size);
            serialize_bytes(stream, data.data(), size);
        } else {
            u32 size = (u32)data.size();
            serialize_uint32(stream, size);
            serialize_bytes(stream, data.data(), size);
        }
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};
struct ServerDestroyEntityMessage : public yojimbo::Message {
    EntityId entity_id;

    ServerDestroyEntityMessage() {
        entity_id = 0;
    }

    template <typename Stream> bool Serialize(Stream& stream) {
        serialize_uint32(stream, entity_id);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

// Naming convention: <Src><Msg> e.g. ServerCreateEntity - Server sending CreateEntity to clients.
enum MessageType { MT_ServerCreateEntity, MT_ServerDestroyEntity, MT_Count };

YOJIMBO_MESSAGE_FACTORY_START(NetMessageFactory, MT_Count);
YOJIMBO_DECLARE_MESSAGE_TYPE(MT_ServerCreateEntity, ServerCreateEntityMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE(MT_ServerDestroyEntity, ServerDestroyEntityMessage);
YOJIMBO_MESSAGE_FACTORY_FINISH();
}  // namespace

NetSystem::NetSystem(Context* context)
    : Object{context}, is_server_{false}, time_{100.0f}, client_{nullptr}, server_{nullptr} {
    yojimbo_logger = subsystem<Logger>();
    InitializeYojimbo();
    yojimbo_log_level(YOJIMBO_LOG_LEVEL_INFO);
    yojimbo_set_printf_function(yojimbo_printf_function);
}

NetSystem::~NetSystem() {
    ShutdownYojimbo();
}

void NetSystem::connect(const String& ip, u16 port) {
    if (isConnected()) {
        disconnect();
    }

    yojimbo::ClientServerConfig config;

    uint8_t privateKey[yojimbo::KeyBytes];
    memset(privateKey, 0, yojimbo::KeyBytes);

    client_ = makeUnique<yojimbo::Client>(yojimbo::GetDefaultAllocator(),
                                          yojimbo::Address{"0.0.0.0"}, config, *this, time_);

    // Decide on client ID.
    u64 clientId = 0;
    yojimbo::random_bytes((uint8_t*)&clientId, 8);
    log().info("Client id is %ull", clientId);

    // Connect to server.
    client_->InsecureConnect(privateKey, clientId, yojimbo::Address{ip.c_str(), port});
    is_server_ = false;
}

void NetSystem::listen(u16 port, u16 max_clients) {
    if (isConnected()) {
        disconnect();
    }

    yojimbo::ClientServerConfig config;

    uint8_t privateKey[yojimbo::KeyBytes];
    memset(privateKey, 0, yojimbo::KeyBytes);

    server_ =
        makeUnique<yojimbo::Server>(yojimbo::GetDefaultAllocator(), privateKey,
                                    yojimbo::Address{"127.0.0.1", port}, config, *this, time_);
    server_->Start(max_clients);
    is_server_ = true;
}

void NetSystem::disconnect() {
    if (isServer()) {
        server_->Stop();
        server_.reset();
    } else if (isClient()) {
        client_->Disconnect();
        client_.reset();
    }
}

void NetSystem::update(float dt) {
    time_ += dt;
    if (server_) {
        server_->SendPackets();
        server_->ReceivePackets();
        for (int i = 0; i < server_->GetNumConnectedClients(); ++i) {
            while (true) {
                yojimbo::Message* message = server_->ReceiveMessage(i, 0);
                if (!message)
                    break;
                switch (message->GetType()) {
                    default:
                        log().warn("Unexpected message received on server: %d", message->GetType());
                }
            }
        }
        server_->AdvanceTime(time_);
    } else if (client_) {
        client_->SendPackets();
        client_->ReceivePackets();
        while (true) {
            yojimbo::Message* message = client_->ReceiveMessage(0);
            if (!message)
                break;
            switch (message->GetType()) {
                case MT_ServerCreateEntity: {
                    // TODO: Create EntitySpawnPipeline and move this code to there.
                    auto create_entity_message = (ServerCreateEntityMessage*)message;
                    BitStream input_bitstream(create_entity_message->data);
                    EntityId entity_id = create_entity_message->entity_id + 100;
                    Entity& entity = subsystem<EntityManager>()->createEntity(entity_id);
                    entity.addComponent<Transform>(Position::origin, Quat::identity, nullptr);
                    entity.addComponent<NetData>(
                        ReplicatedPropertyList{ReplicatedProperty::bind(&Transform::position)});
                    entity.component<NetData>()->deserialise(input_bitstream);
                    log().info("Created replicated entity %d at %d %d %d", entity_id,
                               entity.transform()->position().x, entity.transform()->position().y,
                               entity.transform()->position().z);
                    break;
                }
                default:
                    log().warn("Unexpected message received on client: %d", message->GetType());
            }
        }
        client_->AdvanceTime(time_);
    }
}

bool NetSystem::isClient() const {
    return client_ && client_->IsConnected();
}

bool NetSystem::isServer() const {
    return server_ && server_->IsRunning();
}

bool NetSystem::isConnected() const {
    return isClient() || isServer();
}

void NetSystem::replicateEntity(const Entity& entity) {
    // assert(isServer());
    assert(entity.hasComponent<NetData>());

    replicated_entities_.insert(entity.id());

    // Send create entity message to clients.
    if (server_) {
        for (int i = 0; i < server_->GetNumConnectedClients(); ++i) {
            sendCreateEntity(i, entity);
        }
    }
}

void NetSystem::sendCreateEntity(int clientIndex, const Entity& entity) {
    auto message =
        (ServerCreateEntityMessage*)server_->CreateMessage(clientIndex, MT_ServerCreateEntity);
    message->entity_id = entity.id();
    // Serialise replicated properties.
    BitStream output_bitstream;
    entity.component<NetData>()->serialise(output_bitstream);
    message->data = output_bitstream.data();
    // TODO: Rewrite InputStream/OutputStream to expose an unreal FArchive like interface, which
    // by default will just write the bytes as-is.
    server_->SendMessage(clientIndex, 0, message);
}

yojimbo::MessageFactory* NetSystem::CreateMessageFactory(yojimbo::Allocator& allocator) {
    return YOJIMBO_NEW(allocator, NetMessageFactory, allocator);
}

void NetSystem::OnServerClientConnected(int clientIndex) {
    // Send replicated entities to client.
    for (auto entity_id : replicated_entities_) {
        Entity* entity = subsystem<EntityManager>()->findEntity(entity_id);
        if (entity) {
            sendCreateEntity(clientIndex, *entity);
        } else {
            log().error("Replicated Entity ID %s missing from EntityManager", entity_id);
        }
    }
}

void NetSystem::OnServerClientDisconnected(int clientIndex) {
}
}  // namespace dw
