/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "io/File.h"
#include "io/FileSystem.h"
#include "resource/ResourceCache.h"

namespace dw {

ResourcePackage::ResourcePackage(Context* ctx, const Path& package) : Object(ctx) {
}

SharedPtr<InputStream> ResourcePackage::getFile(const ResourcePath& path_within_location) {
    log().warn("ResourcePackage::getFile() - Loading from a ResourcePackage is unimplemented.");
    return nullptr;
}

ResourceFilesystemPath::ResourceFilesystemPath(Context* ctx, const Path& path) : Object(ctx), path_{path} {
    
}

SharedPtr<InputStream> ResourceFilesystemPath::getFile(const ResourcePath& path_within_location) {
    //Path full_path = binding.second.get<Path>() + 
    Path full_path = path_ + path_within_location;
    log().info("Loading resource from filesystem at " + full_path);
    if (subsystem<FileSystem>()->fileExists(full_path)) {
        return makeShared<File>(context(), full_path, FileMode::Read);
    } else {
        return nullptr;
    }
}

ResourceCache::ResourceCache(Context* context) : Object(context) {
}

ResourceCache::~ResourceCache() {
}

void ResourceCache::addPath(const ResourcePath& binding, const Path& path)
{
    resource_location_bindings_.emplace(makePair(binding, std::move(makeUnique<ResourceFilesystemPath>(context(), path))));
}

void ResourceCache::addPackage(const ResourcePath& binding, UniquePtr<ResourcePackage>&& package)
{
    resource_location_bindings_.emplace(makePair(binding, package));
}

SharedPtr<InputStream> ResourceCache::getResourceData(const ResourcePath& resource_path) {
    ResourcePath simplified_path = simplifyAbsolutePath(resource_path);
    for (auto& binding : resource_location_bindings_)
    {
        const ResourcePath& bind_location = binding.first;
        // If prefix is in a resource location binding, load from that resource location.
        if (simplified_path.substr(0, bind_location.size()) == binding.first)
        {
            return binding.second->getFile(simplified_path.substr(bind_location.size()));
        }
    }
    return nullptr;
}
}  // namespace dw
