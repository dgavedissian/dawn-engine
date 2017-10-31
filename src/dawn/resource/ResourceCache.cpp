/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "io/File.h"
#include "io/FileSystem.h"
#include "resource/ResourceCache.h"

namespace dw {

ResourceCache::ResourceCache(Context* context) : Object(context) {
}

ResourceCache::~ResourceCache() {
}

void ResourceCache::addPath(const ResourcePath& binding, const Path& path)
{
    resource_location_bindings_[binding] = path;
}

void ResourceCache::addPackage(const ResourcePath& binding, SharedPtr<ResourcePackage> package)
{
    resource_location_bindings_[binding] = package;
}

SharedPtr<File> ResourceCache::getFile(const ResourcePath& resource_path) {
    ResourcePath simplified_path = simplifyAbsolutePath(resource_path);
    for (auto binding : resource_location_bindings_)
    {
        const ResourcePath& bind_location = binding.first;
        // If prefix is in a resource location binding, load from that resource location.
        if (simplified_path.substr(0, bind_location.size()) == binding.first)
        {
            if (binding.second.is<Path>()) {
                Path full_path = binding.second.get<Path>() + simplified_path.substr(bind_location.size());
                log().info("Loading resource from " + full_path + " (inside binding " + bind_location + ")");
                if (subsystem<FileSystem>()->fileExists(full_path)) {
                    return makeShared<File>(context(), full_path, FileMode::Read);
                }
            } else if (binding.second.is<SharedPtr<ResourcePackage>>())
            {
                log().warn("ResourceCache::getFile() - Loading from a ResourcePackage is unimplemented.");
            }
        }
    }
    return SharedPtr<File>();
}
}  // namespace dw
