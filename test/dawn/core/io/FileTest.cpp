/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Testing.h"
#include "core/io/File.h"

class FileTest : public ::testing::Test {
public:
    void SetUp() override {
        context_ = new dw::Context("", "");
        context_->addModule<dw::Logger>();
        context_->addModule<dw::FileSystem>();
    }

    void TearDown() override {}

protected:
    dw::Context* context_;
};

TEST_F(FileTest, ReadByte) {
    dw::File file(context_, "core/io/testfiles/data", dw::FileMode::Read);
    dw::u32 data;
    dw::u32 bytes_read = file.readData(&data, sizeof(dw::u32));
    EXPECT_EQ(0x806f6f66, data);
    EXPECT_EQ(4, bytes_read);
    EXPECT_EQ(true, file.eof());
}

TEST_F(FileTest, ReadSingleLine) {
    dw::File file(context_, "core/io/testfiles/a.txt", dw::FileMode::Read);
    EXPECT_EQ("test", dw::stream::read<dw::String>(file));
}

TEST_F(FileTest, ReadMultiLine) {
    dw::File file(context_, "core/io/testfiles/b.txt", dw::FileMode::Read);
    EXPECT_EQ("line1\nline2\n", dw::stream::read<dw::String>(file));
}

TEST_F(FileTest, WriteThenRead) {
    auto& fs = *context_->module<dw::FileSystem>();

    auto filename = fs.tempDir() + "/write_test";
    dw::u32 data = 0xdeadbeef;
    dw::u32 in_data;

    {
        dw::File out_file(context_, filename, dw::FileMode::Write);
        out_file.writeData(&data, sizeof(dw::u32));
    }

    {
        dw::File in_file(context_, filename, dw::FileMode::Read);
        in_file.readData(&in_data, sizeof(dw::u32));
    }

    fs.deleteFile(filename);

    EXPECT_EQ(data, in_data);
}

TEST_F(FileTest, WriteThenAppendThenRead) {
    auto& fs = *context_->module<dw::FileSystem>();

    auto filename = fs.tempDir() + "/append_test";
    dw::u32 data = 0xdeadbeef;

    // Write
    {
        dw::File out_file(context_, filename, dw::FileMode::Write);
        out_file.writeData(&data, sizeof(dw::u32));
    }

    // Append
    {
        dw::File append_file(context_, filename, dw::FileMode::Write | dw::FileMode::Append);
        append_file.writeData(&data, sizeof(dw::u32));
    }

    // Read
    dw::u64 in_data;
    {
        dw::File in_file(context_, filename, dw::FileMode::Read);
        in_file.readData(&in_data, sizeof(dw::u64));
    }

    fs.deleteFile(filename);

    EXPECT_EQ(0xdeadbeefdeadbeef, in_data);
}
