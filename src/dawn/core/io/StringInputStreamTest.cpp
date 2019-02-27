/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Testing.h"
#include "core/io/StringInputStream.h"

class StringInputStreamTest : public ::testing::Test {
public:
    void SetUp() override {
        context_ = new dw::Context("", "");
        context_->addModule<dw::Logger>();
    }

    void TearDown() override {}

protected:
    dw::Context* context_;
};

TEST_F(StringInputStreamTest, ReadSingleLine) {
    dw::StringInputStream string{"test"};
    EXPECT_EQ("test", dw::stream::read<dw::String>(string));
}

TEST_F(StringInputStreamTest, ReadMultiLine) {
    dw::StringInputStream string{"line1\nline2\n"};
    EXPECT_EQ("line1\nline2\n", dw::stream::read<dw::String>(string));
}
