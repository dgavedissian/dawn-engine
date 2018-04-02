/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "Path.h"
#include "InputStream.h"
#include "OutputStream.h"

namespace dw {

struct FileMode {
    enum {
        Read = 0b0001,
        Write = 0b0010,

        // Other options
        Append = 0b0100,

        ReadWrite = Read | Write
    };
};

class DW_API File : public Object, public InputStream, public OutputStream {
public:
    DW_OBJECT(File);

    File(Context* context);

    File(Context* context, const Path& path, int mode = FileMode::Read);
    ~File() override;

    u32 readData(void* dest, u32 size) override;
    void seek(u64 position) override;

    u32 writeData(const void* src, u32 size) override;

    bool open(const Path& path, int mode);
    void close();

private:
    FILE* handle_;
    int mode_;

    String fileModeMapper(int mode);
};
}  // namespace dw
