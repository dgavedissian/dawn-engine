/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "FileSystem.h"
#include "core/StringUtils.h"

#if DW_PLATFORM == DW_WIN32
#include "platform/Windows.h"
#else
#define MAX_PATH 256
#include <unistd.h>
#include <sys/stat.h>
#endif

#include <cstdio>

namespace dw {

FileSystem::FileSystem(Context* context) : Module(context) {
}

FileSystem::~FileSystem() {
}

bool FileSystem::setWorkingDir(const Path& path) const {
#if DW_PLATFORM == DW_WIN32
    if (::SetCurrentDirectoryA(path.c_str()) == FALSE) {
        // TODO(David): Error handling
        log().error("Failed to change directory to %s", path);
        return false;
    }
#else
    if (::chdir(path.c_str()) != 0) {
        log().error("Failed to change directory to %s (errno %i)", path, errno);
        return false;
    }
#endif

    return true;
}

Path FileSystem::workingDir() const {
    char path[MAX_PATH];
    path[0] = 0;
#if DW_PLATFORM == DW_WIN32
    ::GetCurrentDirectoryA(MAX_PATH, path);
#else
    ::getcwd(path, MAX_PATH);
#endif
    return Path(path);
}

Path FileSystem::tempDir() const {
#if DW_PLATFORM == DW_WIN32
    char dir[MAX_PATH + 1];
    ::GetTempPathA(MAX_PATH + 1, dir);
    return Path(dir);
#else
    char const* dir = ::getenv("TMPDIR");
    if (dir == 0) {
        dir = "/tmp";
    }
    return Path(dir);
#endif
}

bool FileSystem::fileExists(const Path& path) const {
#if DW_PLATFORM == DW_WIN32
    DWORD attributes = ::GetFileAttributesA(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES || attributes & FILE_ATTRIBUTE_DIRECTORY) {
        return false;
    }
#else
    struct stat st;
    if (::stat(path.c_str(), &st) || st.st_mode & S_IFDIR) {
        return false;
    }
#endif

    return true;
}

bool FileSystem::rename(const Path& oldname, const Path& newname) const {
    // TODO(David): Error handling by errno
    return ::rename(oldname.c_str(), newname.c_str()) != 0;
}

bool FileSystem::deleteFile(const Path& path) const {
    return ::remove(path.c_str()) != 0;
}
}  // namespace dw
