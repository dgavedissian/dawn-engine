/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "DawnEngine.h"
#include "ecs/EntityManager.h"
#include "ecs/SystemManager.h"
#include "io/File.h"
#include "renderer/MeshBuilder.h"
#include "renderer/Material.h"
#include "resource/ResourceCache.h"
#include "scene/Node.h"
#include "scene/Parent.h"

using namespace dw;

struct PositionData : public Component {
    float x;
    float y;
    float z;
};

class Test : public System {
public:
    Test(Context* context) : System{context} {
        SupportsComponents<PositionData>();
    }
    void ProcessEntity(Entity* e) override {
        auto pos = *e->component<PositionData>();
        pos.x = 3;
        pos.y = 5;
        pos.z += 1;
    }
};

class Sandbox : public App {
public:
    DW_OBJECT(Sandbox);

    void init(int argc, char** argv) override {
        subsystem<FileSystem>()->setWorkingDir("media");

        File file(context_, "sandbox/test.txt", FileMode::Read);
        log().info("File contents: %s", stream::read<u8>(file));

        auto rc = subsystem<ResourceCache>();

        // Create a node.
        SharedPtr<Node> node = makeShared<Node>(context());
        SharedPtr<Material> material =
            makeShared<Material>(context(), rc->get<ShaderProgram>("sandbox/sphere.vs"),
                                 rc->get<ShaderProgram>("sandbox/sphere.fs"));
        node->SetRenderable(
            MeshBuilder(context()).withNormals(false).withTexcoords(false).createSphere(10.0f));
        node->GetRenderable()->setMaterial(material);

        auto sm = subsystem<SystemManager>();
        auto em = subsystem<EntityManager>();
        sm->addSystem<Test>();
        em->createEntity()->addComponent<PositionData>(0.0f, 0.0f, 0.0f).addComponent<Parent>(1);
    }

    void update(float dt) override {
    }

    void shutdown() override {
    }

    String gameName() override {
        return "Sandbox";
    }

    String gameVersion() override {
        return "1.0.0";
    }
};

DW_IMPLEMENT_MAIN(Sandbox);
