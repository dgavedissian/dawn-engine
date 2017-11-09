/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "io/BitStream.h"

#if DW_PLATFORM == DW_WIN32
#include <io.h>
#else
#include <sys/stat.h>
#endif

namespace dw {

File::File(Context* context) : Object{context}, handle_{nullptr}, mode_{0} {
}

File::File(Context* context, const Path& path, int mode) : File(context) {
    open(path, mode);
}

File::~File() {
    close();
}

u32 File::read(void* dest, u32 size) {
    if (!handle_) {
        return 0;
    }

    size_t ret = fread(dest, size, 1, handle_);
    if (ret != 1) {
        // TODO(David): Correct error handling
        if (feof(handle_)) {
            ERROR_FATAL("Unexpected EOF reached");
        } else if (ferror(handle_)) {
            ERROR_FATAL("Unexpected error occurred");
        }
        return 0;
    }

    position_ += size;

    return size;
}

void File::seek(u64 position) {
    if (!handle_) {
        return;
    }

    // If writing, then sparse seeks are allowed (where the cursor can be positioned past the end of
    // the file)
    if (!(mode_ & FileMode::Write) && position > size_) {
        position = size_;
    }

    fseek(handle_, static_cast<long>(position), SEEK_SET);
    position_ = position;
}

u32 File::write(const void* src, u32 size) {
    if (!handle_) {
        return 0;
    }

    fwrite(src, size, 1, handle_);
    position_ += size;
    if (position_ > size_) {
        size_ = position_;
    }

    return size;
}

bool File::open(const Path& path, int mode) {
    String modeStr = fileModeMapper(mode);
    handle_ = fopen(path.c_str(), modeStr.c_str());
    if (!handle_) {
        log().error("Failed to open file: %s", path);
        return false;
    }

// Determine file size.
// Based off
// https://www.securecoding.cert.org/confluence/display/c/FIO19-C.+Do+not+use+fseek%28%29+and+ftell%28%29+to+compute+the+size+of+a+regular+file
#if DW_PLATFORM == DW_WIN32
    size_ = _filelengthi64(_fileno(handle_));
#else
    struct stat stbuf;
    if ((fstat(fileno(handle_), &stbuf) != 0) || (!S_ISREG(stbuf.st_mode))) {
        // TODO Handle error
    }
    size_ = stbuf.st_size;
#endif

    return true;
}

void File::close() {
    if (handle_) {
        fclose(handle_);
        handle_ = nullptr;
    }
}

String File::fileModeMapper(int mode) {
    assert(mode != 0);

    String str_mode;
    if (mode & FileMode::Read) {
        if (mode & FileMode::Write) {
            str_mode = mode & FileMode::Append ? "a+b" : "w+b";
        } else {
            str_mode = "rb";
        }
    }

    if (mode & FileMode::Write) {
        str_mode = mode & FileMode::Append ? "ab" : "wb";
    }

    assert(str_mode.length() > 0);
    return str_mode;
}
}  // namespace dw
