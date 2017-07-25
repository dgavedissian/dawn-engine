/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "DawnEngine.h"
#include "renderer/MeshBuilder.h"
#include "core/Timer.h"

using namespace dw;

template <typename T> class Example : public App {
public:
    DW_OBJECT(Example<T>);

    void init(int, char**) override {
        example_ = makeUnique<T>(context_, engine_);
        example_->start();
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

        // Render example.
        example_->render();
    }

    void update(float) override {
    }

    void shutdown() override {
        example_->stop();
        example_.reset();
    }

    String gameName() override {
        return "RendererTest";
    }

    String gameVersion() override {
        return "1.0.0";
    }

private:
    UniquePtr<T> example_;
};

#define TEST_CLASS_NAME(test_name) test_name##Test
#define TEST_CLASS(test_name) class TEST_CLASS_NAME(test_name) : public Object
#define TEST_BODY(test_name)                                                    \
    \
public:                                                                         \
    DW_OBJECT(TEST_CLASS_NAME(test_name));                                      \
    TEST_CLASS_NAME(test_name)                                                  \
    (Context * context, const Engine* engine)                                   \
        : Object{context}, r{context->subsystem<Renderer>()}, engine_{engine} { \
    }                                                                           \
    Renderer* r;                                                                \
    const Engine* engine_
#define TEST_IMPLEMENT_MAIN(test_name) DW_IMPLEMENT_MAIN(Example<TEST_CLASS_NAME(test_name)>)

// Utils
namespace util {
Mat4 createProjMatrix(float n, float f, float fov_y, float aspect) {
    float tangent = static_cast<float>(tan(fov_y * dw::M_DEGTORAD_OVER_2));  // tangent of half fovY
    float v = n * tangent * 2;  // half height of near plane
    float h = v * aspect;       // half width of near plane
    return Mat4::OpenGLPerspProjRH(n, f, h, v);
}

uint createFullscreenQuad(Renderer* r, VertexBufferHandle& vb) {
    // clang-format off
    float vertices[] = {
    	// Position   | UV
        -1.0f, -1.0f, 0.0f, 0.0f,
    	3.0f,  -1.0f, 2.0f, 0.0f,
    	-1.0f,  3.0f, 0.0f, 2.0f};
    // clang-format on
    VertexDecl decl;
    decl.begin()
        .add(VertexDecl::Attribute::Position, 2, VertexDecl::AttributeType::Float)
        .add(VertexDecl::Attribute::TexCoord0, 2, VertexDecl::AttributeType::Float)
        .end();
    vb = r->createVertexBuffer(vertices, sizeof(vertices), decl);
    return 3;
}
}  // namespace util

TEST_CLASS(BasicVertexBuffer) {
    TEST_BODY(BasicVertexBuffer);

    VertexBufferHandle vb_;
    ProgramHandle program_;

    void start() {
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

    void render() {
        r->setViewClear(0, {0.0f, 0.0f, 0.2f, 1.0f});
        r->setVertexBuffer(vb_);
        r->submit(0, program_, 3);
    }

    void stop() {
        r->deleteProgram(program_);
        r->deleteVertexBuffer(vb_);
    }
};

TEST_CLASS(BasicIndexBuffer) {
    TEST_BODY(BasicIndexBuffer);

    VertexBufferHandle vb_;
    IndexBufferHandle ib_;
    ProgramHandle program_;

    void start() {
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

    void render() {
        r->setVertexBuffer(vb_);
        r->setIndexBuffer(ib_);
        r->submit(0, program_, 6);
    }

    void stop() {
        r->deleteProgram(program_);
        r->deleteVertexBuffer(vb_);
    }
};

TEST_CLASS(Textured3DCube) {
    TEST_BODY(Textured3DCube);

    SharedPtr<CustomMesh> box_;
    ProgramHandle program_;

    // Uses the higher level wrapper which provides loading from files.
    UniquePtr<Texture> texture_resource_;

    void start() {
        subsystem<FileSystem>()->setWorkingDir("../media/renderer-test");

        // Load shaders.
        File vs_file{context(), "shaders/cube_textured.vs"};
        String vs_source = dw::stream::read<String>(vs_file);
        File fs_file{context(), "shaders/cube_textured.fs"};
        String fs_source = dw::stream::read<String>(fs_file);

        auto vs = r->createShader(ShaderType::Vertex, vs_source);
        auto fs = r->createShader(ShaderType::Fragment, fs_source);
        program_ = r->createProgram();
        r->attachShader(program_, vs);
        r->attachShader(program_, fs);
        r->linkProgram(program_);
        r->setUniform("wall_sampler", 0);
        r->submit(0, program_);

        // Load texture.
        File texture_file{context(), "wall.jpg"};
        texture_resource_ = makeUnique<Texture>(context());
        texture_resource_->beginLoad(texture_file);
        texture_resource_->endLoad();

        // Create box.
        box_ = MeshBuilder{context()}.withNormals(true).withTexcoords(true).createBox(10.0f);
    }

    void render() {
        // Calculate matrices.
        static float angle = 0.0f;
        angle += M_PI / 3.0f * engine_->frameTime();  // 60 degrees per second.
        Mat4 model = Mat4::Translate(Vec3{0.0f, 0.0f, -50.0f}).ToFloat4x4() * Mat4::RotateY(angle);
        static Mat4 view = Mat4::identity;
        static Mat4 proj = util::createProjMatrix(0.1f, 1000.0f, 60.0f, 1280.0f / 800.0f);
        r->setUniform("model_matrix", model);
        r->setUniform("mvp_matrix", proj * view * model);
        r->setUniform("light_direction", Vec3{1.0f, 1.0f, 1.0f}.Normalized());

        // Set vertex buffer and submit.
        r->setViewClear(0, {0.0f, 0.0f, 0.2f, 1.0f});
        r->setTexture(texture_resource_->internalHandle(), 0);
        auto task = box_->draw(model);
        r->setVertexBuffer(task.primitive.vb);
        r->setIndexBuffer(task.primitive.ib);
        r->submit(0, program_, task.primitive.count);
    }

    void stop() {
        r->deleteProgram(program_);
    }
};

TEST_CLASS(PostProcessing) {
    TEST_BODY(PostProcessing);

    SharedPtr<CustomMesh> box_;
    ProgramHandle box_program_;

    VertexBufferHandle fsq_vb_;
    ProgramHandle post_process_;
    FrameBufferHandle fb_handle_;

    void start() {
        subsystem<FileSystem>()->setWorkingDir("../media/renderer-test");

        // Load shaders.
        File vs_file{context(), "shaders/cube_solid.vs"};
        String vs_source = dw::stream::read<String>(vs_file);
        File fs_file{context(), "shaders/cube_solid.fs"};
        String fs_source = dw::stream::read<String>(fs_file);

        auto vs = r->createShader(ShaderType::Vertex, vs_source);
        auto fs = r->createShader(ShaderType::Fragment, fs_source);
        box_program_ = r->createProgram();
        r->attachShader(box_program_, vs);
        r->attachShader(box_program_, fs);
        r->linkProgram(box_program_);

        // Create box.
        box_ = MeshBuilder{context()}.withNormals(true).withTexcoords(true).createBox(10.0f);

        // Create full screen quad.
        util::createFullscreenQuad(r, fsq_vb_);

        // Set up frame buffer.
        fb_handle_ = r->createFrameBuffer(1280, 800, TextureFormat::RGB8);

        // Load post process shader.
        File pp_vs_file{context(), "shaders/post_process.vs"};
        String pp_vs_source = dw::stream::read<String>(pp_vs_file);
        File pp_fs_file{context(), "shaders/post_process.fs"};
        String pp_fs_source = dw::stream::read<String>(pp_fs_file);
        auto pp_vs = r->createShader(ShaderType::Vertex, pp_vs_source);
        auto pp_fs = r->createShader(ShaderType::Fragment, pp_fs_source);
        post_process_ = r->createProgram();
        r->attachShader(post_process_, pp_vs);
        r->attachShader(post_process_, pp_fs);
        r->linkProgram(post_process_);
        r->setUniform("in_sampler", 0);
        r->submit(0, post_process_);
    }

    void render() {
        // Calculate matrices.
        static float angle = 0.0f;
        angle += M_PI / 3.0f * engine_->frameTime();  // 60 degrees per second.
        Mat4 model = Mat4::Translate(Vec3{0.0f, 0.0f, -50.0f}).ToFloat4x4() * Mat4::RotateY(angle);
        static Mat4 view = Mat4::identity;
        static Mat4 proj = util::createProjMatrix(0.1f, 1000.0f, 60.0f, 1280.0f / 800.0f);
        r->setUniform("model_matrix", model);
        r->setUniform("mvp_matrix", proj * view * model);
        r->setUniform("light_direction", Vec3{1.0f, 1.0f, 1.0f}.Normalized());

        // Set up views.
        r->setViewClear(0, {0.0f, 0.0f, 0.2f, 1.0f});
        r->setViewFrameBuffer(0, fb_handle_);
        r->setViewClear(1, {0.0f, 0.2f, 0.0f, 1.0f});
        r->setViewFrameBuffer(1, FrameBufferHandle{0});

        // Set vertex buffer and submit.
        auto task = box_->draw(model);
        r->setVertexBuffer(task.primitive.vb);
        r->setIndexBuffer(task.primitive.ib);
        r->submit(0, box_program_, task.primitive.count);

        // Draw fb.
        r->setTexture(r->getFrameBufferTexture(fb_handle_, 0), 0);
        r->setVertexBuffer(fsq_vb_);
        r->submit(1, post_process_, 3);
    }

    void stop() {
        r->deleteProgram(post_process_);
        r->deleteVertexBuffer(fsq_vb_);
        r->deleteProgram(box_program_);
    }
};

TEST_CLASS(DeferredShading) {
    TEST_BODY(DeferredShading);

    SharedPtr<CustomMesh> sphere_;
    ProgramHandle cube_program_;

    // Uses the higher level wrapper which provides loading from files.
    UniquePtr<Texture> texture_resource_;

    ProgramHandle post_process_;

    VertexBufferHandle fsq_vb_;
    FrameBufferHandle gbuffer_;

    void start() {
        subsystem<FileSystem>()->setWorkingDir("../media/renderer-test");

        // Load shaders.
        File vs_file{context(), "shaders/cube_gbuffer.vs"};
        String vs_source = dw::stream::read<String>(vs_file);
        File fs_file{context(), "shaders/cube_gbuffer.fs"};
        String fs_source = dw::stream::read<String>(fs_file);

        auto vs = r->createShader(ShaderType::Vertex, vs_source);
        auto fs = r->createShader(ShaderType::Fragment, fs_source);
        cube_program_ = r->createProgram();
        r->attachShader(cube_program_, vs);
        r->attachShader(cube_program_, fs);
        r->linkProgram(cube_program_);
        r->setUniform("wall_sampler", 0);
        r->submit(0, cube_program_);

        // Create box.
        sphere_ = MeshBuilder{context_}.withNormals(true).withTexcoords(true).createSphere(10.0f);

        // Load texture.
        File texture_file{context(), "wall.jpg"};
        texture_resource_ = makeUnique<Texture>(context());
        texture_resource_->beginLoad(texture_file);
        texture_resource_->endLoad();

        // Create full screen quad.
        util::createFullscreenQuad(r, fsq_vb_);

        // Set up frame buffer.
        uint width = 1280, height = 800;
        auto format = TextureFormat::RGBA32F;
        gbuffer_ = r->createFrameBuffer({r->createTexture2D(width, height, format, nullptr, 0),
                                         r->createTexture2D(width, height, format, nullptr, 0),
                                         r->createTexture2D(width, height, format, nullptr, 0)});

        // Load post process shader.
        File pp_vs_file{context(), "shaders/post_process.vs"};
        String pp_vs_source = dw::stream::read<String>(pp_vs_file);
        File pp_fs_file{context(), "shaders/deferred_directional_light_pass.fs"};
        String pp_fs_source = dw::stream::read<String>(pp_fs_file);
        auto pp_vs = r->createShader(ShaderType::Vertex, pp_vs_source);
        auto pp_fs = r->createShader(ShaderType::Fragment, pp_fs_source);
        post_process_ = r->createProgram();
        r->attachShader(post_process_, pp_vs);
        r->attachShader(post_process_, pp_fs);
        r->linkProgram(post_process_);
        r->setUniform("gb0_sampler", 0);
        r->setUniform("gb1_sampler", 1);
        r->setUniform("gb2_sampler", 2);
        r->submit(0, post_process_);
    }

    void render() {
        // Set up views.
        r->setViewClear(0, {0.0f, 0.0f, 1.0f, 1.0f});
        r->setViewFrameBuffer(0, gbuffer_);
        r->setViewClear(1, {0.0f, 0.2f, 0.0f, 1.0f});
        r->setViewFrameBuffer(1, FrameBufferHandle{0});

        // Calculate matrices.
        static float angle = 0.0f;
        angle += M_PI / 3.0f * engine_->frameTime();  // 60 degrees per second.
        Mat4 model = Mat4::Translate(Vec3{0.0f, 0.0f, -50.0f}).ToFloat4x4() * Mat4::RotateY(angle);
        static Mat4 view = Mat4::identity;
        static Mat4 proj = util::createProjMatrix(0.1f, 1000.0f, 60.0f, 1280.0f / 800.0f);
        r->setUniform("model_matrix", model);
        r->setUniform("mvp_matrix", proj * view * model);

        // Set vertex buffer and submit.
        auto task = sphere_->draw(Mat4::identity);
        r->setVertexBuffer(task.primitive.vb);
        r->setIndexBuffer(task.primitive.ib);
        r->setTexture(texture_resource_->internalHandle(), 0);
        r->submit(0, cube_program_, task.primitive.count);

        // Draw fb.
        r->setTexture(r->getFrameBufferTexture(gbuffer_, 0), 0);
        r->setTexture(r->getFrameBufferTexture(gbuffer_, 1), 1);
        r->setTexture(r->getFrameBufferTexture(gbuffer_, 2), 2);
        r->setVertexBuffer(fsq_vb_);
        r->setUniform("light_direction", Vec3{1.0f, 1.0f, 1.0f}.Normalized());
        r->submit(1, post_process_, 3);
    }

    void stop() {
        r->deleteProgram(post_process_);
        r->deleteVertexBuffer(fsq_vb_);
        r->deleteProgram(cube_program_);
    }
};

TEST_IMPLEMENT_MAIN(Textured3DCube);
