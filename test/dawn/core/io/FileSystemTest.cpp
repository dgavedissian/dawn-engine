/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Testing.h"
#include "core/io/FileSystem.h"

class FileSystemTest : public ::testing::Test {
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

TEST_F(FileSystemTest, FileDoesExist) {
    EXPECT_TRUE(context_->module<dw::FileSystem>()->fileExists("core/io/testfiles/a.txt"));
}

TEST_F(FileSystemTest, FileDoesNotExist) {
    EXPECT_FALSE(context_->module<dw::FileSystem>()->fileExists("core/io/testfiles/missing.txt"));
}
