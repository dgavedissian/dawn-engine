/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "io/FileSystem.h"

#include <sys/stat.h>

/*
#if DW_PLATFORM == DW_WIN32
// TODO(David): Required windows headers
#else
#include <unistd.h>
#endif
 */

namespace dw {

const u32 MAX_PATH = 256;

FileSystem::FileSystem(Context* context) : Object(context) {
}

FileSystem::~FileSystem() {
}

bool FileSystem::setWorkingDir(const Path& path) {
#if DW_PLATFORM == DW_WIN32
    if (SetCurrentDirectoryA(path.c_str()) == FALSE)
    {
        LOGERR << "Failed to change directory to " << path;
        return false;
    }
#else
    if (chdir(path.c_str()) != 0)
    {
        LOGERR << "Failed to change directory to " << path;
        return false;
    }
#endif

    return true;
}

Path FileSystem::getWorkingDir() const {
    char path[MAX_PATH];
    path[0] = 0;
#if DW_PLATFORM == DW_WIN32
    GetCurrentDirectoryA(MAX_PATH, path);
#else
    getcwd(path, MAX_PATH);
#endif
    return Path(path);
}

bool FileSystem::fileExists(const Path &path) {
#if DW_PLATFORM == DW_WIN32
    DWORD attributes = GetFileAttributesA(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES || attributes & FILE_ATTRIBUTE_DIRECTORY)
        return false;
#else
    struct stat st;
    if (stat(fixedName.CString(), &st) || st.st_mode & S_IFDIR)
        return false;
#endif

    return true;
}

}