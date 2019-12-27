/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "core/Collections.h"
#include "core/io/File.h"
#include "resource/Resource.h"

namespace dw {
using ResourcePath = String;

Pair<String, Path> parseResourcePath(const ResourcePath& resource_path);

class DW_API ResourceLocation {
public:
    virtual ~ResourceLocation() = default;
    virtual Result<SharedPtr<InputStream>> getFile(const ResourcePath& path_within_location) = 0;
};

class DW_API ResourcePackage : public Object, public ResourceLocation {
public:
    DW_OBJECT(ResourcePackage);

    ResourcePackage(Context* ctx, const Path& package);

    Result<SharedPtr<InputStream>> getFile(const ResourcePath& path_within_location) override;
};

class DW_API ResourceFilesystemPath : public Object, public ResourceLocation {
public:
    DW_OBJECT(ResourceFilesystemPath);

    ResourceFilesystemPath(Context* ctx, const Path& path);

    Result<SharedPtr<InputStream>> getFile(const ResourcePath& path_within_location) override;

private:
    Path path_;
};

class DW_API ResourceCache : public Module {
public:
    DW_OBJECT(ResourceCache);

    ResourceCache(Context* context);
    ~ResourceCache();

    void addPath(const String& package, const Path& path);
    void addPackage(const String& package, UniquePtr<ResourcePackage> file);

    // Raw API to read resource data.
    Result<SharedPtr<InputStream>> loadRaw(const ResourcePath& resource_path);

    template <typename T>
    SharedPtr<T> addCustomResource(const ResourcePath& resource_path, SharedPtr<T> resource) {
        String name(resource_path);

        if (!resource) {
            log().warn("NULL resource provided at {}. Skipping.", name);
            return nullptr;
        }

        if (resource_cache_.find(name) != resource_cache_.end()) {
            log().warn("Found an existing resource at {}. Replacing with an instance of '{}'", name,
                       resource->typeName());
        }
        resource_cache_[name] = resource;
        return resource;
    }

    template <typename T>
    Result<SharedPtr<T>, String> get(const ResourcePath& resource_path,
                                     const String& parameters = "") {
        String name(resource_path);
        if (!parameters.empty()) {
            name += "?" + parameters;
        }

        // If the resource already exists, return it.
        auto it = resource_cache_.find(name);
        if (it != resource_cache_.end()) {
            return staticPointerCast<T>((*it).second);
        }

        // Load the file which contains this resource data.
        auto resource_data = loadRaw(resource_path);
        if (!resource_data) {
            return makeError(str::format("Cannot find resource {}. Reason: {}", resource_path,
                                         resource_data.error()));
        }
        SharedPtr<T> resource = makeShared<T>(context());
        resource_cache_.emplace(name, resource);
        log().info("Loading asset '{}'", resource_path);
        auto load_result = resource->load(resource_path, *resource_data.value().get());
        if (!load_result) {
            return makeError(str::format("Failed to load resource {}. Reason: {}", resource_path,
                                         load_result.error()));
        }
        return resource;
    }

    template <typename T>
    SharedPtr<T> getUnchecked(const ResourcePath& resource_path, const String& parameters = "") {
        auto result = get<T>(resource_path);
        if (!result) {
            log().error("getUnchecked failed: {}", result.error());
            std::terminate();
        }
        return *result;
    }

private:
    Map<String, UniquePtr<ResourceLocation>> resource_packages_;
    HashMap<String, SharedPtr<Resource>> resource_cache_;
};
}  // namespace dw
