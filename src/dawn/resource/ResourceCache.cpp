/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "io/File.h"
#include "io/FileSystem.h"
#include "resource/ResourceCache.h"

namespace dw {
Pair<String, Path> parseResourcePath(const ResourcePath& resource_path) {
    auto split_point = resource_path.find(':');
    if (split_point == String::npos) {
        return {"unknown_package", ""};
    }
    return {resource_path.substr(0, split_point), "/" + resource_path.substr(split_point + 1)};
};

ResourcePackage::ResourcePackage(Context* ctx, const Path& package) : Object(ctx) {
}

SharedPtr<InputStream> ResourcePackage::getFile(const ResourcePath& path_within_location) {
    log().warn("ResourcePackage::getFile() - Loading from a ResourcePackage is unimplemented.");
    return nullptr;
}

ResourceFilesystemPath::ResourceFilesystemPath(Context* ctx, const Path& path)
    : Object(ctx), path_{path} {
}

SharedPtr<InputStream> ResourceFilesystemPath::getFile(const ResourcePath& path_within_location) {
    Path full_path = path_ + path_within_location;
    log().info("Loading resource from filesystem at " + full_path);
    if (subsystem<FileSystem>()->fileExists(full_path)) {
        return makeShared<File>(context(), full_path, FileMode::Read);
    } else {
        return nullptr;
    }
}

ResourceCache::ResourceCache(Context* context) : Subsystem(context) {
}

ResourceCache::~ResourceCache() {
}

void ResourceCache::addPath(const String& package, const Path& path) {
    resource_packages_.emplace(
        makePair(package, makeUnique<ResourceFilesystemPath>(context(), path)));
}

void ResourceCache::addPackage(const String& package, UniquePtr<ResourcePackage> file) {
    resource_packages_.emplace(makePair(package, std::move(file)));
}

SharedPtr<InputStream> ResourceCache::getResourceData(const ResourcePath& resource_path) {
    // Parse resource path.
    auto path = parseResourcePath(resource_path);
    String package = path.first;

    // Look up package and get the file within that package.
    auto package_it = resource_packages_.find(package);
    if (package_it == resource_packages_.end()) {
        // Unknown package.
        log().error("Attempting to load from unknown package: %s - Full path: %s", package,
                    resource_path);
        return nullptr;
    }
    return package_it->second->getFile(simplifyAbsolutePath(path.second));
}
}  // namespace dw
