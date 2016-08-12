/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "io/InputStream.h"
#include "io/OutputStream.h"

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

class File : public Object, public InputStream, public OutputStream {
public:
    DW_OBJECT(File);

    File(Context* context);
    File(Context* context, const Path& path, int mode = FileMode::Read);
    ~File() override;

    u32 read(void* dest, u32 size) override;
    void seek(u64 position) override;

    u32 write(const void* src, u32 size) override;

    void open(const Path& path, int mode);
    void close();

private:
    FILE* mHandle;
    int mMode;

    String fileModeMapper(int mode);
};
}
