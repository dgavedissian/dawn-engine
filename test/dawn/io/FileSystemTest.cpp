/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Testing.h"
#include "io/FileSystem.h"

class FileSystemTest : public ::testing::Test {
public:
    void SetUp() override {
        context_ = new dw::Context("", "");
        context_->addModule<dw::Logger>();
        auto& fs = *context_->addModule<dw::FileSystem>();
    }

    void TearDown() override {}

protected:
    dw::Context* context_;
};

TEST_F(FileSystemTest, FileDoesExist) {
    EXPECT_TRUE(context_->module<dw::FileSystem>()->fileExists("io/testfiles/a.txt"));
}

TEST_F(FileSystemTest, FileDoesNotExist) {
    EXPECT_FALSE(context_->module<dw::FileSystem>()->fileExists("io/testfiles/missing.txt"));
}
