/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "core/io/File.h"
#include "core/io/FileSystem.h"
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

Result<SharedPtr<InputStream>, String> ResourcePackage::getFile(
    const ResourcePath& path_within_location) {
    return {"ResourcePackage::getFile() - Loading from a ResourcePackage is unimplemented."};
}

ResourceFilesystemPath::ResourceFilesystemPath(Context* ctx, const Path& path)
    : Object(ctx), path_{path} {
}

Result<SharedPtr<InputStream>, String> ResourceFilesystemPath::getFile(
    const ResourcePath& path_within_location) {
    Path full_path = path_ + path_within_location;
    log().info("Loading resource from filesystem at " + full_path);
    if (module<FileSystem>()->fileExists(full_path)) {
        return {makeShared<File>(context(), full_path, FileMode::Read)};
    }
    return {str::format("File %s does not exist.", full_path)};
}

ResourceCache::ResourceCache(Context* context) : Module(context) {
}

ResourceCache::~ResourceCache() {
}

void ResourceCache::addPath(const String& package, const Path& path) {
#ifndef DW_EMSCRIPTEN
    const auto& real_path = path;
#else
    // TODO(David): Don't hard code resource paths in this way.
    const auto& real_path = "/media/" + package;
#endif
    resource_packages_.emplace(
        makePair(package, makeUnique<ResourceFilesystemPath>(context(), real_path)));
}

void ResourceCache::addPackage(const String& package, UniquePtr<ResourcePackage> file) {
    resource_packages_.emplace(makePair(package, std::move(file)));
}

Result<SharedPtr<InputStream>, String> ResourceCache::getResourceData(
    const ResourcePath& resource_path) {
    // Parse resource path.
    auto path = parseResourcePath(resource_path);
    String package = path.first;

    // Look up package and get the file within that package.
    auto package_it = resource_packages_.find(package);
    if (package_it == resource_packages_.end()) {
        return {str::format("Attempting to load from unknown package: %s - Full path: %s", package,
                            resource_path)};
    }
    return package_it->second->getFile(simplifyAbsolutePath(path.second));
}
}  // namespace dw
