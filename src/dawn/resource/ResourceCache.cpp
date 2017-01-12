/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "io/File.h"
#include "io/FileSystem.h"
#include "resource/ResourceCache.h"

namespace dw {

ResourceCache::ResourceCache(Context *context) : Object(context) {
}

ResourceCache::~ResourceCache() {
}

void ResourceCache::addResourcePath(const Path &path) {
    mResourcePaths.push_back(path);
}

SharedPtr<File> ResourceCache::getFile(const Path &filename) {
    for (const Path &resPath : mResourcePaths) {
        Path fullPath = resPath + "/" + filename;
        if (getSubsystem<FileSystem>()->fileExists(fullPath)) {
            return makeShared<File>(getContext(), fullPath, FileMode::Read);
        }
    }

    return SharedPtr<File>();
}
}
