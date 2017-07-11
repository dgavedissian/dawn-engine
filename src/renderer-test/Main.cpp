/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "DawnEngine.h"
#include <core/Timer.h>

using namespace dw;

class BasicVertexBufferTest : public App {
public:
    DW_OBJECT(BasicVertexBufferTest);

    VertexBufferHandle vb_;
    ProgramHandle program_;

    Renderer* r;

    void init(int argc, char** argv) override {
        r = subsystem<Renderer>();
        subsystem<FileSystem>()->setWorkingDir("../media/renderer-test");

        // Load shaders.
        File vs_file{context(), "shaders/test.vs"};
        String vs_source = dw::stream::read<String>(vs_file);
        File fs_file{context(), "shaders/test.fs"};
        String fs_source = dw::stream::read<String>(fs_file);

        auto vs = r->createShader(ShaderType::Vertex, vs_source);
        auto fs = r->createShader(ShaderType::Fragment, fs_source);
        program_ = r->createProgram();
        r->attachShader(program_, vs);
        r->attachShader(program_, fs);
        r->linkProgram(program_);

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
            .add(VertexDecl::Attribute::Colour, 4, VertexDecl::AttributeType::Uint8, true)
            .end();
        vb_ = r->createVertexBuffer(vertices, sizeof(vertices), decl);
    }

    void render() override {
        r->setVertexBuffer(vb_);
        r->submit(program_, 3);
    }

    void update(float dt) override {
    }

    void shutdown() override {
        r->deleteProgram(program_);
        r->deleteVertexBuffer(vb_);
    }

    String gameName() override {
        return "RendererTest";
    }

    String gameVersion() override {
        return "1.0.0";
    }
};

class BasicIndexBufferTest : public App {
public:
    DW_OBJECT(BasicIndexBufferTest);

    VertexBufferHandle vb_;
    IndexBufferHandle ib_;
    ProgramHandle program_;

    Renderer* r;

    void init(int argc, char** argv) override {
        r = subsystem<Renderer>();
        subsystem<FileSystem>()->setWorkingDir("../media/renderer-test");

        // Load shaders.
        File vs_file{context(), "shaders/test.vs"};
        String vs_source = dw::stream::read<String>(vs_file);
        File fs_file{context(), "shaders/test.fs"};
        String fs_source = dw::stream::read<String>(fs_file);

        auto vs = r->createShader(ShaderType::Vertex, vs_source);
        auto fs = r->createShader(ShaderType::Fragment, fs_source);
        program_ = r->createProgram();
        r->attachShader(program_, vs);
        r->attachShader(program_, fs);
        r->linkProgram(program_);

        float vertices[] = {
            -0.5f, 0.5f,  1.0f, 0.0f, 0.0f,  // Top-left
            0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  // Top-right
            0.5f,  -0.5f, 0.0f, 0.0f, 1.0f,  // Bottom-right
            -0.5f, -0.5f, 1.0f, 1.0f, 1.0f   // Bottom-left
        };
        VertexDecl decl;
        decl.begin()
            .add(VertexDecl::Attribute::Position, 2, VertexDecl::AttributeType::Float)
            .add(VertexDecl::Attribute::Colour, 3, VertexDecl::AttributeType::Float)
            .end();
        vb_ = r->createVertexBuffer(vertices, sizeof(vertices), decl);

        GLuint elements[] = {0, 1, 2, 2, 3, 0};
        ib_ = r->createIndexBuffer(elements, sizeof(elements), IndexBufferType::U32);
    }

    void render() override {
        r->setVertexBuffer(vb_);
        r->setIndexBuffer(ib_);
        r->submit(program_, 6);
    }

    void update(float dt) override {
    }

    void shutdown() override {
        r->deleteProgram(program_);
        r->deleteVertexBuffer(vb_);
    }

    String gameName() override {
        return "RendererTest";
    }

    String gameVersion() override {
        return "1.0.0";
    }
};

class Textured3DCubeTest : public App {
public:
    DW_OBJECT(Textured3DCubeTest);

    VertexBufferHandle vb_;
    uint vertex_count_;
    ProgramHandle program_;

    // Uses the higher level wrapper which provides loading from files.
    UniquePtr<Texture> texture_resource_;

    Renderer* r;

    void init(int argc, char** argv) override {
        r = subsystem<Renderer>();
        subsystem<FileSystem>()->setWorkingDir("../media/renderer-test");

        // Load shaders.
        File vs_file{context(), "shaders/test3d.vs"};
        String vs_source = dw::stream::read<String>(vs_file);
        File fs_file{context(), "shaders/test3d.fs"};
        String fs_source = dw::stream::read<String>(fs_file);

        auto vs = r->createShader(ShaderType::Vertex, vs_source);
        auto fs = r->createShader(ShaderType::Fragment, fs_source);
        program_ = r->createProgram();
        r->attachShader(program_, vs);
        r->attachShader(program_, fs);
        r->linkProgram(program_);
        r->setUniform("wall_texture", 0);

        // Load texture.
        File texture_file{context(), "wall.jpg"};
        texture_resource_ = makeUnique<Texture>(context());
        texture_resource_->beginLoad(texture_file);
        texture_resource_->endLoad();

        // Create box.
        vertex_count_ = createBox(10.0f, vb_);
    }

    void render() override {
        // Display FPS.
        static double accumulated_time = 1.0;
        accumulated_time += engine_->frameTime();
        if (accumulated_time > 1.0) {
            log().info("Elapsed time: %s, FPS: %s", engine_->frameTime(),
                       1.0 / engine_->frameTime());
            accumulated_time = 0;
        }

        // Calculate matrices.
        static float angle = 0.0f;
        angle += M_PI / 3.0f * engine_->frameTime();  // 60 degrees per second.
        Mat4 model = Mat4::Translate(Vec3{0.0f, 0.0f, -50.0f}).ToFloat4x4() * Mat4::RotateY(angle);
        static Mat4 view = Mat4::identity;
        float n = 0.1f;
        float f = 1000.0f;
        float fovY = 60.0f;
        float aspect = 1280.0f / 800.0f;
        float tangent = (float)tan(fovY * dw::M_DEGTORAD_OVER_2);  // tangent of half fovY
        float v = n * tangent * 2;                                 // half height of near plane
        float h = v * aspect;                                      // half width of near plane
        static Mat4 projection = Mat4::OpenGLPerspProjRH(n, f, h, v);
        r->setUniform("model_matrix", model);
        r->setUniform("mvp_matrix", projection * view * model);
        r->setUniform("light_direction", Vec3{1.0f, 1.0f, 1.0f}.Normalized());

        // Set vertex buffer and submit.
        r->setVertexBuffer(vb_);
        r->setTexture(texture_resource_->internalHandle(), 0);
        r->submit(program_, vertex_count_);
    }

    void update(float dt) override {
    }

    void shutdown() override {
        r->deleteProgram(program_);
        r->deleteVertexBuffer(vb_);
    }

    String gameName() override {
        return "RendererTest";
    }

    String gameVersion() override {
        return "1.0.0";
    }

    uint createBox(float halfSize, VertexBufferHandle& vb) {
        float vertices[] = {
            // Position						| UVs		  | Normals
            -halfSize, -halfSize, -halfSize, 0.0f,  0.0f,  -1.0f, 0.0f, 0.0f,
            halfSize,  -halfSize, -halfSize, 0.0f,  0.0f,  -1.0f, 1.0f, 0.0f,
            halfSize,  halfSize,  -halfSize, 0.0f,  0.0f,  -1.0f, 1.0f, 1.0f,
            halfSize,  halfSize,  -halfSize, 0.0f,  0.0f,  -1.0f, 1.0f, 1.0f,
            -halfSize, halfSize,  -halfSize, 0.0f,  0.0f,  -1.0f, 0.0f, 1.0f,
            -halfSize, -halfSize, -halfSize, 0.0f,  0.0f,  -1.0f, 0.0f, 0.0f,

            -halfSize, -halfSize, halfSize,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
            halfSize,  -halfSize, halfSize,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
            halfSize,  halfSize,  halfSize,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
            halfSize,  halfSize,  halfSize,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
            -halfSize, halfSize,  halfSize,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
            -halfSize, -halfSize, halfSize,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

            -halfSize, halfSize,  halfSize,  -1.0f, 0.0f,  0.0f,  0.0f, 0.0f,
            -halfSize, halfSize,  -halfSize, -1.0f, 0.0f,  0.0f,  1.0f, 0.0f,
            -halfSize, -halfSize, -halfSize, -1.0f, 0.0f,  0.0f,  1.0f, 1.0f,
            -halfSize, -halfSize, -halfSize, -1.0f, 0.0f,  0.0f,  1.0f, 1.0f,
            -halfSize, -halfSize, halfSize,  -1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
            -halfSize, halfSize,  halfSize,  -1.0f, 0.0f,  0.0f,  0.0f, 0.0f,

            halfSize,  halfSize,  halfSize,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
            halfSize,  halfSize,  -halfSize, 1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
            halfSize,  -halfSize, -halfSize, 1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
            halfSize,  -halfSize, -halfSize, 1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
            halfSize,  -halfSize, halfSize,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            halfSize,  halfSize,  halfSize,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,

            -halfSize, -halfSize, -halfSize, 0.0f,  -1.0f, 0.0f,  0.0f, 0.0f,
            halfSize,  -halfSize, -halfSize, 0.0f,  -1.0f, 0.0f,  1.0f, 0.0f,
            halfSize,  -halfSize, halfSize,  0.0f,  -1.0f, 0.0f,  1.0f, 1.0f,
            halfSize,  -halfSize, halfSize,  0.0f,  -1.0f, 0.0f,  1.0f, 1.0f,
            -halfSize, -halfSize, halfSize,  0.0f,  -1.0f, 0.0f,  0.0f, 1.0f,
            -halfSize, -halfSize, -halfSize, 0.0f,  -1.0f, 0.0f,  0.0f, 0.0f,

            -halfSize, halfSize,  -halfSize, 0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
            halfSize,  halfSize,  -halfSize, 0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
            halfSize,  halfSize,  halfSize,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
            halfSize,  halfSize,  halfSize,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
            -halfSize, halfSize,  halfSize,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
            -halfSize, halfSize,  -halfSize, 0.0f,  1.0f,  0.0f,  0.0f, 0.0f};

        VertexDecl decl;
        decl.begin()
            .add(VertexDecl::Attribute::Position, 3, VertexDecl::AttributeType::Float)
            .add(VertexDecl::Attribute::Normal, 3, VertexDecl::AttributeType::Float)
            .add(VertexDecl::Attribute::TexCoord0, 2, VertexDecl::AttributeType::Float)
            .end();

        vb = r->createVertexBuffer(vertices, sizeof(vertices), decl);
        return 36;
    }
};

DW_IMPLEMENT_MAIN(Textured3DCubeTest);
