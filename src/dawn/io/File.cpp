/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "io/File.h"

namespace dw {

File::File(Context* context) : Object(context), handle_(nullptr), mode_(0) {
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

    // Determine file size
    fseek(handle_, 0, SEEK_END);
    size_ = (u64)ftell(handle_);
    fseek(handle_, 0, SEEK_SET);

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
}  // namespace dw
