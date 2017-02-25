/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "DawnEngine.h"
#include "io/File.h"
#include "renderer/MeshBuilder.h"
#include "renderer/Material.h"
#include "resource/ResourceCache.h"
#include "scene/Node.h"

using namespace dw;

class Sandbox : public App {
public:
    DW_OBJECT(Sandbox);

    void init(int argc, char** argv) override {
        getSubsystem<FileSystem>()->setWorkingDir("media");

        File file(mContext, "sandbox/test.txt", FileMode::Read);
        getLog().info("File contents: %s", stream::read<u8>(file));

        auto rc = getSubsystem<ResourceCache>();

        // Create a node.
        SharedPtr<Node> node = makeShared<Node>(getContext());
        SharedPtr<Material> material =
            makeShared<Material>(getContext(), rc->get<ShaderProgram>("sandbox/sphere.vs"),
                                 rc->get<ShaderProgram>("sandbox/sphere.fs"));
        node->SetRenderable(
            MeshBuilder(getContext()).withNormals(false).withTexcoords(false).createSphere(10.0f));
        node->setMaterial(material);
    }

    void update(float dt) override {
    }

    void shutdown() override {
    }

    dw::String getGameName() override {
        return "Sandbox";
    }

    dw::String getGameVersion() override {
        return "1.0.0";
    }
};

DW_IMPLEMENT_MAIN(Sandbox);
