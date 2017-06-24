/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "DawnEngine.h"
#include "ecs/EntityManager.h"
#include "ecs/SystemManager.h"
#include "io/File.h"
#include "renderer/MeshBuilder.h"
#include "renderer/GLProgram.h"
#include "resource/ResourceCache.h"
#include "scene/Node.h"
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

class Sandbox : public App {
public:
    DW_OBJECT(Sandbox);

    UniquePtr<Node> node;

    ProgramHandle program;
    VertexBufferHandle vb;

    void init(int argc, char** argv) override {
        File file(context_, "media/sandbox/test.txt", FileMode::Read);
        log().info("File contents: %s", stream::read<u8>(file));

        auto rc = subsystem<ResourceCache>();
        assert(rc);
        rc->addResourcePath("media/base");
        rc->addResourcePath("media/sandbox");

        // Create a node.
        node = makeUnique<Node>(context());
        SharedPtr<GLProgram> material =
            makeShared<GLProgram>(context(), rc->get<GLShader>("shaders/bin/sphere.vs"),
                                  rc->get<GLShader>("shaders/bin/sphere.fs"));
        node->setRenderable(
            MeshBuilder(context()).withNormals(false).withTexcoords(false).createSphere(10.0f));
        node->renderable()->setMaterial(material);

        auto sm = subsystem<SystemManager>();
        auto em = subsystem<EntityManager>();
        // sm->addSystem<Test>();
        // em->createEntity().addComponent<PositionData>(0.0f, 0.0f, 0.0f).addComponent<Parent>(1);
        em->createEntity()
            .addComponent<RenderableComponent>(node->renderable())
            .addComponent<Transform>(Position(), Quat::identity);

        // Do render stuff.
        auto& r = *subsystem<Renderer>();
        subsystem<FileSystem>()->setWorkingDir("/Users/dga/Projects/dawnengine/media/sandbox");

        File vs_file{context(), "shaders/bin/test.vs"};
        String vs_source = dw::stream::read<String>(vs_file);
        File fs_file{context(), "shaders/bin/test.fs"};
        String fs_source = dw::stream::read<String>(fs_file);

        auto vs = r.createShader(ShaderType::Vertex, vs_source);
        auto fs = r.createShader(ShaderType::Fragment, fs_source);
        program = r.createProgram();
        r.attachShader(program, vs);
        r.attachShader(program, fs);
        r.linkProgram(program);

        struct Vertex {
            float x;
            float y;
            u32 colour;
        };
        Vertex vertices[] = {
            // Little-endian, so colours are 0xAABBGGRR.
            {0.0f, 0.5f, 0xff0000ff},   // Vertex 1: Red
            {0.5f, -0.5f, 0xff00ff00},  // Vertex 2: Green
            {-0.5f, -0.5f, 0xffff0000}  // Vertex 3: Blue
        };
        VertexDecl decl;
        decl.begin()
            .add(VertexDecl::Attribute::Position, 2, VertexDecl::AttributeType::Float)
            .add(VertexDecl::Attribute::Colour, 4, VertexDecl::AttributeType::Uint8)
            .end();
        log().warn("%s", sizeof(vertices));
        vb = r.createVertexBuffer(vertices, sizeof(vertices), decl);
    }

    void update(float dt) override {
        auto& r = *subsystem<Renderer>();
        r.setVertexBuffer(vb);
        r.submit(program, 3);
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
