/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "net/NetSystem.h"

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
inline int GetNumBitsForMessage(uint16_t sequence) {
    static int messageBitsArray[] = {1,  320, 120, 4, 256, 45,  11, 13, 101, 100, 84,
                                     95, 203, 2,   3, 8,   512, 5,  3,  7,   50};
    const int modulus = sizeof(messageBitsArray) / sizeof(int);
    const int index = sequence % modulus;
    return messageBitsArray[index];
}

struct TestMessage : public yojimbo::Message {
    uint16_t sequence;

    TestMessage() {
        sequence = 0;
    }

    template <typename Stream> bool Serialize(Stream& stream) {
        serialize_bits(stream, sequence, 16);

        int numBits = GetNumBitsForMessage(sequence);
        int numWords = numBits / 32;
        uint32_t dummy = 0;
        for (int i = 0; i < numWords; ++i)
            serialize_bits(stream, dummy, 32);
        int numRemainderBits = numBits - numWords * 32;
        if (numRemainderBits > 0)
            serialize_bits(stream, dummy, numRemainderBits);

        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

enum TestMessageType { TEST_MESSAGE, NUM_TEST_MESSAGE_TYPES };

YOJIMBO_MESSAGE_FACTORY_START(TestMessageFactory, NUM_TEST_MESSAGE_TYPES);
YOJIMBO_DECLARE_MESSAGE_TYPE(TEST_MESSAGE, TestMessage);
YOJIMBO_MESSAGE_FACTORY_FINISH();

class YojimboAdapter : public yojimbo::Adapter {
public:
    yojimbo::MessageFactory* CreateMessageFactory(yojimbo::Allocator& allocator) {
        return YOJIMBO_NEW(allocator, TestMessageFactory, allocator);
    }
};
YojimboAdapter g_adapter;
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
                                          yojimbo::Address{"0.0.0.0"}, config, g_adapter, time_);

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
                                    yojimbo::Address{"127.0.0.1", port}, config, g_adapter, time_);
    server_->Start(max_clients);
    is_server_ = true;
}

void NetSystem::disconnect() {
    if (isServer()) {
        server_->Stop();
        server_.reset();
    } else {
        client_->Disconnect();
        client_.reset();
    }
}

void NetSystem::update(float dt) {
    if (!isConnected()) {
        return;
    }

    time_ += dt;
    if (isServer()) {
        server_->SendPackets();
        server_->ReceivePackets();
        server_->AdvanceTime(time_);
    } else {
        client_->SendPackets();
        client_->ReceivePackets();
        client_->AdvanceTime(time_);
    }
}

bool NetSystem::isServer() const {
    return is_server_;
}

bool NetSystem::isConnected() const {
    return isServer() ? server_ != nullptr : client_ != nullptr;
}
}  // namespace dw
