/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "io/File.h"

namespace dw {

File::File(Context* context) : Object(context), mHandle(nullptr), mMode(0) {
}

File::File(Context* context, const Path& path, int mode) : File(context) {
    open(path, mode);
}

File::~File() {
    close();
}

u32 File::read(void* dest, u32 size) {
    if (!mHandle)
        return 0;

    size_t ret = fread(dest, size, 1, mHandle);
    if (ret != 1) {
        // TODO(David): Correct error handling
        if (feof(mHandle)) {
            ERROR_FATAL("Unexpected EOF reached");
        } else if (ferror(mHandle)) {
            ERROR_FATAL("Unexpected error occurred");
        }
        return 0;
    }

    mPosition += size;

    return size;
}

void File::seek(u64 position) {
    if (!mHandle)
        return;

    // If writing, then sparse seeks are allowed (where the cursor can be positioned past the end of
    // the file)
    if (!(mMode & FileMode::Write) && position > mSize)
        position = mSize;

    fseek(mHandle, position, SEEK_SET);
    mPosition = position;
}

u32 File::write(const void* src, u32 size) {
    if (!mHandle)
        return 0;

    fwrite(src, size, 1, mHandle);
    mPosition += size;
    if (mPosition > mSize)
        mSize = mPosition;

    return size;
}

bool File::open(const Path& path, int mode) {
    String modeStr = fileModeMapper(mode);
    mHandle = fopen(path.c_str(), modeStr.c_str());
    if (!mHandle) {
        getLog().error("Failed to open file: %s", path);
        return false;
    }

    // Determine file size
    fseek(mHandle, 0, SEEK_END);
    mSize = (u64)ftell(mHandle);
    fseek(mHandle, 0, SEEK_SET);

    return true;
}

void File::close() {
    if (mHandle) {
        fclose(mHandle);
        mHandle = nullptr;
    }
}

String File::fileModeMapper(int mode) {
    assert(mode != 0);

    if (mode & FileMode::Read) {
        if (mode & FileMode::Write) {
            return (mode & FileMode::Append) ? "a+b" : "w+b";
        } else {
            return "rb";
        }
    }

    if (mode & FileMode::Write) {
        return (mode & FileMode::Append) ? "ab" : "wb";
    }

    // This should never happen
    assert(false && "Invalid file mode specified");
    return "rb";
}
}
