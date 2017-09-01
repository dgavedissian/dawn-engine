/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "DawnEngine.h"
#include "ecs/EntityManager.h"
#include "ecs/SystemManager.h"
#include "renderer/Program.h"
#include "renderer/MeshBuilder.h"
#include "resource/ResourceCache.h"
#include "scene/Parent.h"
#include "scene/Transform.h"
#include "renderer/Mesh.h"

using namespace dw;

class Sandbox : public App {
public:
    DW_OBJECT(Sandbox);

    Entity* object;
    Entity* camera;

    void init(int argc, char** argv) override {
        auto rc = subsystem<ResourceCache>();
        assert(rc);
        rc->addResourceLocation("../media/base");
        rc->addResourceLocation("../media/sandbox");

        // Create a material.
        auto material = makeShared<Material>(
            context(), makeShared<Program>(context(), rc->get<VertexShader>("ship.vs"),
                                           rc->get<FragmentShader>("ship.fs")));
        material->program()->setUniform("light_direction", Vec3{1.0f, 1.0f, 1.0f}.Normalized());

        auto renderable = rc->get<Mesh>("model/core-large.mesh.xml");
        renderable->setMaterial(material);
        auto sphere = rc->get<Mesh>("model/side-wing.mesh.xml");
        sphere->setMaterial(material);

        // Create entities.
        auto em = subsystem<EntityManager>();
        object = &em->createEntity()
                      .addComponent<Transform>(Position{-10.0f, 0.0f, 0.0f}, Quat::identity)
                      .addComponent<RenderableComponent>(renderable);
        em->createEntity()
            .addComponent<Transform>(Position{8.0f, 0.0f, 0.0f}, Quat::identity)
            .addComponent<Parent>(object->id())
            .addComponent<RenderableComponent>(sphere);
        em->createEntity()
            .addComponent<Transform>(Position{-8.0f, 0.0f, 0.0f}, Quat::identity)
            .addComponent<Parent>(object->id())
            .addComponent<RenderableComponent>(sphere);

        // Create a camera.
        camera = &em->createEntity()
                      .addComponent<Transform>(Position{0.0f, 0.0f, 50.0f}, Quat::identity)
                      .addComponent<Camera>(0.1f, 1000.0f, 60.0f, 1280.0f / 800.0f);
    }

    void update(float dt) override {
        static float angle = 0.0f;
        angle += dt;
        // camera->component<Transform>()->position.x = sin(angle) * 30.0f;
        object->component<Transform>()->orientation = Quat::RotateY(angle);
    }

    void render() override {
        subsystem<Renderer>()->setViewClear(0, {0.0f, 0.0f, 0.2f, 1.0f});
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
