/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "DawnEngine.h"
#include "ecs/EntityManager.h"
#include "ecs/SystemManager.h"
#include "renderer/MeshBuilder.h"
#include "renderer/Program.h"
#include "resource/ResourceCache.h"
#include "scene/Parent.h"
#include "scene/Transform.h"

using namespace dw;

/*
struct PositionData : public Component {
    PositionData(float x, float y, float z) : x{x}, y{y}, z{z} {
    }
    float x;
    float y;
    float z;
};

class Test : public System {
public:
    Test(Context* context) : System{context} {
        supportsComponents<PositionData>();
    }
    void processEntity(Entity& e) override {
        auto pos = *e.component<PositionData>();
        pos.x = 3;
        pos.y = 5;
        pos.z += 1;
    }
};
 */

class MeshViewer : public App {
public:
    DW_OBJECT(MeshViewer);

    void init(int argc, char** argv) override {
        auto rc = subsystem<ResourceCache>();
        assert(rc);
        rc->addResourcePath("media/base");
        rc->addResourcePath("media/sandbox");

        // Create a node.
        auto material = makeShared<Material>(
            context(),
            makeShared<Program>(context(), rc->get<VertexShader>("shaders/bin/sphere.vs"),
                                rc->get<FragmentShader>("shaders/bin/sphere.fs")));
        auto renderable = MeshBuilder(context()).normals(false).normals(false).createSphere(10.0f);
        renderable->setMaterial(material);

        auto sm = subsystem<SystemManager>();
        auto em = subsystem<EntityManager>();
        // sm->addSystem<Test>();
        // em->createEntity().addComponent<PositionData>(0.0f, 0.0f, 0.0f).addComponent<Parent>(1);
        em->createEntity()
            .addComponent<RenderableComponent>(renderable)
            .addComponent<Transform>(Position{}, Quat::identity);
    }

    void update(float dt) override {
    }

    void shutdown() override {
    }

    String gameName() override {
        return "MeshViewer";
    }

    String gameVersion() override {
        return "1.0.0";
    }
};

DW_IMPLEMENT_MAIN(Sandbox);
