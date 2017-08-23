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

class Sandbox : public App {
public:
    DW_OBJECT(Sandbox);

    void init(int argc, char** argv) override {
        auto rc = subsystem<ResourceCache>();
        assert(rc);
        rc->addResourcePath("../media/base");
        rc->addResourcePath("../media/sandbox");

        // Create an object.
        auto material = makeShared<Material>(
            context(), makeShared<Program>(context(), rc->get<VertexShader>("ship.vs"),
                                           rc->get<FragmentShader>("ship.fs")));
        auto renderable = MeshBuilder(context()).normals(false).normals(false).createSphere(10.0f);
        renderable->setMaterial(material);

        auto sm = subsystem<SystemManager>();
        auto em = subsystem<EntityManager>();
        em->createEntity()
            .addComponent<Transform>(Position{0.0f, 0.0f, -30.0f}, Quat::identity)
            .addComponent<RenderableComponent>(renderable);

        // Create a camera.
        em->createEntity()
            .addComponent<Transform>(Position{0.0f, 0.0f, 30.0f}, Quat::identity)
            .addComponent<Camera>(0.1f, 1000.0f, 60.0f, 1280.0f / 800.0f);
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
