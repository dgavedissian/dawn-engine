/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "DawnEngine.h"
#include "renderer/MeshBuilder.h"
#include "core/Timer.h"
#include "scene/CTransform.h"
#include "renderer/CCamera.h"
#include "renderer/SceneGraph.h"
#include "renderer/RenderPipeline.h"

using namespace dw;

class Example : public Object {
public:
    DW_OBJECT(Example);

    Example(Context* ctx, const Engine* engine, const GameSession* session)
        : Object(ctx), r{ctx->module<Renderer>()->rhi()}, engine_{engine}, session_{session} {
    }

    u16 width() const {
        return context()->config().at("window_width").get<u16>();
    }

    u16 height() const {
        return context()->config().at("window_height").get<u16>();
    }

    virtual void start() = 0;
    virtual void render() = 0;
    virtual void stop() = 0;

    rhi::RHIRenderer* r;
    const Engine* engine_;
    const GameSession* session_;
};

template <typename... T> class ExampleAppContainer : public App {
public:
    DW_OBJECT(ExampleAppContainer<T...>);

    Vector<SharedPtr<Example>> examples_;
    Example* current_example_;
    int frame_id_;

    ExampleAppContainer() : current_example_(nullptr), frame_id_(0) {
    }

    void init(const CommandLine&) override {
        auto session = makeUnique<GameSession>(context(), GameSessionInfo{});
        examples_ = {makeShared<T>(context_, engine_, session.get())...};
        engine_->addSession(std::move(session));
    }

    void render(float) override {
        // Store FPS history.
        float current_fps = engine_->framesPerSecond();
        static const int FPS_HISTORY_COUNT = 100;
        static float fps_history[FPS_HISTORY_COUNT];
        static double accumulated_time = 0.0;
        accumulated_time += engine_->frameTime();
        if (accumulated_time > 1.0 / 20.0) {
            accumulated_time = 0.0;
            for (int i = 1; i < FPS_HISTORY_COUNT; ++i) {
                fps_history[i - 1] = fps_history[i];
            }
            fps_history[FPS_HISTORY_COUNT - 1] = current_fps;
        }

        // Display FPS information.
        ImGui::SetNextWindowPos({0, 0});
        ImGui::SetNextWindowSize({280, 130});
        ImGui::Begin("FPS");
        ImGui::Text("FPS: %f", current_fps);
        ImGui::PlotLines("", fps_history, FPS_HISTORY_COUNT, 0, nullptr, 0.0f, 1000.0f, {250, 80});
        ImGui::End();

        // Display example list.
        ImGui::SetNextWindowPos({0, 130});
        ImGui::SetNextWindowSize({180, 250});
        ImGui::Begin("Examples");
        for (auto& example : examples_) {
            if (ImGui::Button(example->typeName().c_str())) {
                if (current_example_) {
                    current_example_->stop();
                }
                current_example_ = example.get();
                current_example_->start();
                frame_id_ = 0;
            }
        }
        ImGui::End();

        // Render example.
        if (current_example_) {
            current_example_->render();
            frame_id_++;
        }
    }

    void update(float) override {
    }

    void shutdown() override {
        if (current_example_) {
            current_example_->stop();
            current_example_ = nullptr;
        }
        examples_.clear();
    }

    String gameName() override {
        return "Examples Viewer";
    }

    String gameVersion() override {
        return DW_VERSION_STR;
    }
};

#define TEST_CLASS(test_name) class test_name : public Example
#define TEST_BODY(test_name)                                                  \
public:                                                                       \
    DW_OBJECT(test_name)                                                      \
    test_name(Context* ctx, const Engine* engine, const GameSession* session) \
        : Example(ctx, engine, session) {                                     \
    }

// Utils
namespace util {
Mat4 createProjMatrix(float n, float f, float fov_y, float aspect) {
    float tangent = static_cast<float>(tan(fov_y * dw::M_DEGTORAD_OVER_2));  // tangent of half fovY
    float v = n * tangent * 2;  // half height of near plane
    float h = v * aspect;       // half width of near plane
    return Mat4::OpenGLPerspProjRH(n, f, h, v);
}

uint createFullscreenQuad(rhi::RHIRenderer* r, rhi::VertexBufferHandle& vb) {
    // clang-format off
    float vertices[] = {
    	// Position   | UV
        -1.0f, -1.0f, 0.0f, 0.0f,
    	3.0f,  -1.0f, 2.0f, 0.0f,
    	-1.0f,  3.0f, 0.0f, 2.0f};
    // clang-format on
    rhi::VertexDecl decl;
    decl.begin()
        .add(rhi::VertexDecl::Attribute::Position, 2, rhi::VertexDecl::AttributeType::Float)
        .add(rhi::VertexDecl::Attribute::TexCoord0, 2, rhi::VertexDecl::AttributeType::Float)
        .end();
    vb = r->createVertexBuffer(Memory(vertices, sizeof(vertices)), decl);
    return 3;
}

rhi::ShaderHandle loadShader(Context* ctx, rhi::ShaderStage type, const String& source_file) {
    static Vector<SharedPtr<Shader>> shader_map;
    SharedPtr<Shader> shader = makeShared<Shader>(ctx, type);
    File file{ctx, source_file};
    shader->beginLoad(source_file, file);
    shader->endLoad();
    shader_map.emplace_back(shader);
    return shader->internalHandle();
}
}  // namespace util

TEST_CLASS(RHIBasicVertexBuffer) {
    TEST_BODY(RHIBasicVertexBuffer);

    rhi::VertexBufferHandle vb_;
    rhi::ProgramHandle program_;

    void start() override {
        module<FileSystem>()->setWorkingDir("../media/examples");

        // Load shaders.
        auto vs = util::loadShader(context(), rhi::ShaderStage::Vertex, "shaders/test.vs");
        auto fs = util::loadShader(context(), rhi::ShaderStage::Fragment, "shaders/test.fs");
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
            {0.0f, 0.5f, 0xff0000ff},    // Vertex 1: Red
            {-0.5f, -0.5f, 0xff00ff00},  // Vertex 2: Green
            {0.5f, -0.5f, 0xffff0000}    // Vertex 3: Blue
        };
        rhi::VertexDecl decl;
        decl.begin()
            .add(rhi::VertexDecl::Attribute::Position, 2, rhi::VertexDecl::AttributeType::Float)
            .add(rhi::VertexDecl::Attribute::Colour, 4, rhi::VertexDecl::AttributeType::Uint8, true)
            .end();
        vb_ = r->createVertexBuffer(Memory(vertices, sizeof(vertices)), decl);
    }

    void render() override {
        r->setViewClear(0, {0.0f, 0.0f, 0.2f, 1.0f});
        r->setVertexBuffer(vb_);
        r->submit(0, program_, 3);
    }

    void stop() override {
        r->deleteProgram(program_);
        r->deleteVertexBuffer(vb_);
    }
};

TEST_CLASS(RHIBasicIndexBuffer) {
    TEST_BODY(RHIBasicIndexBuffer);

    rhi::VertexBufferHandle vb_;
    rhi::IndexBufferHandle ib_;
    rhi::ProgramHandle program_;

    void start() override {
        module<FileSystem>()->setWorkingDir("../media/examples");

        // Load shaders.
        auto vs = util::loadShader(context(), rhi::ShaderStage::Vertex, "shaders/test.vs");
        auto fs = util::loadShader(context(), rhi::ShaderStage::Fragment, "shaders/test.fs");
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
        rhi::VertexDecl decl;
        decl.begin()
            .add(rhi::VertexDecl::Attribute::Position, 2, rhi::VertexDecl::AttributeType::Float)
            .add(rhi::VertexDecl::Attribute::Colour, 3, rhi::VertexDecl::AttributeType::Float)
            .end();
        vb_ = r->createVertexBuffer(Memory(vertices, sizeof(vertices)), decl);

        u32 elements[] = {0, 2, 1, 2, 0, 3};
        ib_ = r->createIndexBuffer(Memory(elements, sizeof(elements)), rhi::IndexBufferType::U32);
    }

    void render() override {
        r->setVertexBuffer(vb_);
        r->setIndexBuffer(ib_);
        r->submit(0, program_, 6);
    }

    void stop() override {
        r->deleteProgram(program_);
        r->deleteVertexBuffer(vb_);
    }
};

TEST_CLASS(RHITransientIndexBuffer) {
    TEST_BODY(RHITransientIndexBuffer);

    rhi::ProgramHandle program_;

    void start() override {
        module<FileSystem>()->setWorkingDir("../media/examples");

        // Load shaders.
        auto vs = util::loadShader(context(), rhi::ShaderStage::Vertex, "shaders/test.vs");
        auto fs = util::loadShader(context(), rhi::ShaderStage::Fragment, "shaders/test.fs");
        program_ = r->createProgram();
        r->attachShader(program_, vs);
        r->attachShader(program_, fs);
        r->linkProgram(program_);
    }

    void render() override {
        r->setViewClear(0, {0.0f, 0.0f, 0.2f, 1.0f});

        static float angle = 0.0f;
        angle += engine_->frameTime();
        float size_multiplier = 1.0f;  //((float)sin(angle) + 1.0f) * 0.25f;
        float vertices[] = {
            -0.5f * size_multiplier, 0.5f * size_multiplier,  1.0f, 0.0f, 0.0f,  // Top-left
            0.5f * size_multiplier,  0.5f * size_multiplier,  0.0f, 1.0f, 0.0f,  // Top-right
            0.5f * size_multiplier,  -0.5f * size_multiplier, 0.0f, 0.0f, 1.0f,  // Bottom-right
            -0.5f * size_multiplier, -0.5f * size_multiplier, 1.0f, 1.0f, 1.0f   // Bottom-left
        };
        rhi::VertexDecl decl;
        decl.begin()
            .add(rhi::VertexDecl::Attribute::Position, 2, rhi::VertexDecl::AttributeType::Float)
            .add(rhi::VertexDecl::Attribute::Colour, 3, rhi::VertexDecl::AttributeType::Float)
            .end();
        auto tvb = r->allocTransientVertexBuffer(sizeof(vertices) / decl.stride(), decl);
        float* vertex_data = (float*)r->getTransientVertexBufferData(tvb);
        memcpy(vertex_data, vertices, sizeof(vertices));

        u16 elements[] = {0, 2, 1, 2, 0, 3};
        auto tib = r->allocTransientIndexBuffer(sizeof(elements) / sizeof(elements[0]));
        u16* index_data = (u16*)r->getTransientIndexBufferData(tib);
        memcpy(index_data, elements, sizeof(elements));

        r->setVertexBuffer(tvb);
        r->setIndexBuffer(tib);
        r->submit(0, program_, 6);
    }

    void stop() override {
        r->deleteProgram(program_);
    }
};

TEST_CLASS(RHITextured3DCube) {
    TEST_BODY(RHITextured3DCube);

    SharedPtr<CustomMeshRenderable> box_;
    rhi::ProgramHandle program_;

    // Uses the higher level wrapper which provides loading from files.
    UniquePtr<Texture> texture_resource_;

    void start() override {
        module<FileSystem>()->setWorkingDir("../media/examples");

        // Load shaders.
        auto vs = util::loadShader(context(), rhi::ShaderStage::Vertex, "shaders/cube_textured.vs");
        auto fs =
            util::loadShader(context(), rhi::ShaderStage::Fragment, "shaders/cube_textured.fs");
        program_ = r->createProgram();
        r->attachShader(program_, vs);
        r->attachShader(program_, fs);
        r->linkProgram(program_);
        r->setUniform("wall_sampler", 0);
        r->submit(0, program_);

        // Load texture.
        File texture_file{context(), "wall.jpg"};
        texture_resource_ = makeUnique<Texture>(context());
        texture_resource_->beginLoad("wall.jpg", texture_file);
        texture_resource_->endLoad();

        // Create box.
        box_ = MeshBuilder{context()}.normals(true).texcoords(true).createBox(10.0f);
    }

    void render() override {
        // Calculate matrices.
        static float angle = 0.0f;
        angle += M_PI / 3.0f * engine_->frameTime();  // 60 degrees per second.
        Mat4 model = Mat4::Translate(Vec3{0.0f, 0.0f, -50.0f}).ToFloat4x4() * Mat4::RotateY(angle);
        static Mat4 view = Mat4::identity;
        static Mat4 proj =
            util::createProjMatrix(0.1f, 1000.0f, 60.0f, static_cast<float>(width()) / height());
        r->setUniform("model_matrix", model);
        r->setUniform("mvp_matrix", proj * view * model);
        r->setUniform("light_direction", Vec3{1.0f, 1.0f, 1.0f}.Normalized());

        // Set vertex buffer and submit.
        r->setViewClear(0, {0.0f, 0.0f, 0.2f, 1.0f});
        r->setTexture(texture_resource_->internalHandle(), 0);
        r->setVertexBuffer(box_->vertexBuffer()->internalHandle());
        r->setIndexBuffer(box_->indexBuffer()->internalHandle());
        r->submit(0, program_, box_->indexBuffer()->indexCount());
    }

    void stop() override {
        r->deleteProgram(program_);
    }
};

TEST_CLASS(RHIPostProcessing) {
    TEST_BODY(RHIPostProcessing);

    SharedPtr<CustomMeshRenderable> box_;
    rhi::ProgramHandle box_program_;

    rhi::VertexBufferHandle fsq_vb_;
    rhi::ProgramHandle post_process_;
    rhi::FrameBufferHandle fb_handle_;

    void start() override {
        module<FileSystem>()->setWorkingDir("../media/examples");

        // Load shaders.
        auto vs = util::loadShader(context(), rhi::ShaderStage::Vertex, "shaders/cube_solid.vs");
        auto fs = util::loadShader(context(), rhi::ShaderStage::Fragment, "shaders/cube_solid.fs");
        box_program_ = r->createProgram();
        r->attachShader(box_program_, vs);
        r->attachShader(box_program_, fs);
        r->linkProgram(box_program_);

        // Create box.
        box_ = MeshBuilder{context()}.normals(true).texcoords(true).createBox(10.0f);

        // Create full screen quad.
        util::createFullscreenQuad(r, fsq_vb_);

        // Set up frame buffer.
        fb_handle_ = r->createFrameBuffer(1280, 800, rhi::TextureFormat::RGB8);

        // Load post process shader.
        auto pp_vs =
            util::loadShader(context(), rhi::ShaderStage::Vertex, "shaders/post_process.vs");
        auto pp_fs =
            util::loadShader(context(), rhi::ShaderStage::Fragment, "shaders/post_process.fs");
        post_process_ = r->createProgram();
        r->attachShader(post_process_, pp_vs);
        r->attachShader(post_process_, pp_fs);
        r->linkProgram(post_process_);
        r->setUniform("in_sampler", 0);
        r->submit(0, post_process_);
    }

    void render() override {
        // Calculate matrices.
        static float angle = 0.0f;
        angle += M_PI / 3.0f * engine_->frameTime();  // 60 degrees per second.
        Mat4 model = Mat4::Translate(Vec3{0.0f, 0.0f, -50.0f}).ToFloat4x4() * Mat4::RotateY(angle);
        static Mat4 view = Mat4::identity;
        static Mat4 proj =
            util::createProjMatrix(0.1f, 1000.0f, 60.0f, static_cast<float>(width()) / height());
        r->setUniform("model_matrix", model);
        r->setUniform("mvp_matrix", proj * view * model);
        r->setUniform("light_direction", Vec3{1.0f, 1.0f, 1.0f}.Normalized());

        // Set up views.
        r->setViewClear(0, {0.0f, 0.0f, 0.2f, 1.0f});
        r->setViewFrameBuffer(0, fb_handle_);
        r->setViewClear(1, {0.0f, 0.2f, 0.0f, 1.0f});
        r->setViewFrameBuffer(1, rhi::FrameBufferHandle{0});

        // Set vertex buffer and submit.
        r->setVertexBuffer(box_->vertexBuffer()->internalHandle());
        r->setIndexBuffer(box_->indexBuffer()->internalHandle());
        r->submit(0, box_program_, box_->indexBuffer()->indexCount());

        // Draw fb.
        r->setTexture(r->getFrameBufferTexture(fb_handle_, 0), 0);
        r->setVertexBuffer(fsq_vb_);
        r->submit(1, post_process_, 3);
    }

    void stop() override {
        r->deleteProgram(post_process_);
        r->deleteVertexBuffer(fsq_vb_);
        r->deleteProgram(box_program_);
    }
};

TEST_CLASS(RHIDeferredShading) {
    TEST_BODY(RHIDeferredShading);

    SharedPtr<CustomMeshRenderable> ground_;
    rhi::ProgramHandle cube_program_;

    // Uses the higher level wrapper which provides loading from files.
    UniquePtr<Texture> texture_resource_;

    rhi::ProgramHandle post_process_;

    rhi::VertexBufferHandle fsq_vb_;
    rhi::FrameBufferHandle gbuffer_;

    class PointLight : public Object {
    public:
        DW_OBJECT(PointLight);

        PointLight(Context* ctx, float radius, const Vec2& screen_size)
            : Object{ctx}, r{module<Renderer>()->rhi()}, light_sphere_radius_{radius * 4} {
            setPosition(Vec3::zero);

            // Load shaders.
            auto vs =
                util::loadShader(context(), rhi::ShaderStage::Vertex, "shaders/light_pass.vs");
            auto fs = util::loadShader(context(), rhi::ShaderStage::Fragment,
                                       "shaders/point_light_pass.fs");
            program_ = r->createProgram();
            r->attachShader(program_, vs);
            r->attachShader(program_, fs);
            r->linkProgram(program_);
            r->setUniform("screen_size", screen_size);
            r->setUniform("gb0", 0);
            r->setUniform("gb1", 1);
            r->setUniform("gb2", 2);
            r->setUniform("radius", radius);
            r->submit(0, program_);
            sphere_ = MeshBuilder{context_}.normals(false).texcoords(false).createSphere(
                light_sphere_radius_, 8, 8);
        }

        ~PointLight() {
            r->deleteProgram(program_);
        }

        void setPosition(const Vec3& position) {
            position_ = position;
            model_ = Mat4::Translate(position);
        }

        void draw(uint view, const Mat4& view_matrix, const Mat4& proj_matrix) {
            Mat4 mvp = proj_matrix * view_matrix * model_;

            // Invert culling when inside the light sphere.
            Vec3 view_space_position = (view_matrix * Vec4(position_, 1.0f)).xyz();
            if (view_space_position.LengthSq() < (light_sphere_radius_ * light_sphere_radius_)) {
                r->setStateCullFrontFace(rhi::CullFrontFace::CW);
            }

            // Disable depth, and enable blending.
            r->setStateDisable(rhi::RenderState::Depth);
            r->setStateEnable(rhi::RenderState::Blending);
            r->setStateBlendEquation(rhi::BlendEquation::Add, rhi::BlendFunc::One,
                                     rhi::BlendFunc::One);

            // Draw sphere.
            r->setVertexBuffer(sphere_->vertexBuffer()->internalHandle());
            r->setIndexBuffer(sphere_->indexBuffer()->internalHandle());
            r->setUniform("mvp_matrix", mvp);
            r->setUniform("light_position", position_);
            r->submit(view, program_, sphere_->indexBuffer()->indexCount());
        }

    private:
        rhi::RHIRenderer* r;
        SharedPtr<CustomMeshRenderable> sphere_;
        rhi::ProgramHandle program_;

        Vec3 position_;
        Mat4 model_;

        float light_sphere_radius_;
    };

    Vector<UniquePtr<PointLight>> point_lights;

    void start() override {
        module<FileSystem>()->setWorkingDir("../media/examples");

        // Load shaders.
        auto vs =
            util::loadShader(context(), rhi::ShaderStage::Vertex, "shaders/object_gbuffer.vs");
        auto fs =
            util::loadShader(context(), rhi::ShaderStage::Fragment, "shaders/object_gbuffer.fs");
        cube_program_ = r->createProgram();
        r->attachShader(cube_program_, vs);
        r->attachShader(cube_program_, fs);
        r->linkProgram(cube_program_);
        r->setUniform("wall_sampler", 0);
        r->setUniform("texcoord_scale", Vec2{10.0f, 10.0f});
        r->submit(0, cube_program_);

        // Create ground.
        ground_ = MeshBuilder{context_}.normals(true).texcoords(true).createPlane(250.0f, 250.0f);

        // Load texture.
        File texture_file{context(), "wall.jpg"};
        texture_resource_ = makeUnique<Texture>(context());
        texture_resource_->beginLoad("wall.jpg", texture_file);
        texture_resource_->endLoad();

        // Create full screen quad.
        util::createFullscreenQuad(r, fsq_vb_);

        // Set up frame buffer.
        auto format = rhi::TextureFormat::RGBA32F;
        gbuffer_ = r->createFrameBuffer({r->createTexture2D(width(), height(), format, Memory()),
                                         r->createTexture2D(width(), height(), format, Memory()),
                                         r->createTexture2D(width(), height(), format, Memory())});

        // Load post process shader.
        auto pp_vs =
            util::loadShader(context(), rhi::ShaderStage::Vertex, "shaders/post_process.vs");
        auto pp_fs = util::loadShader(context(), rhi::ShaderStage::Fragment,
                                      "shaders/deferred_ambient_light_pass.fs");
        post_process_ = r->createProgram();
        r->attachShader(post_process_, pp_vs);
        r->attachShader(post_process_, pp_fs);
        r->linkProgram(post_process_);
        r->setUniform("gb0_sampler", 0);
        r->setUniform("gb1_sampler", 1);
        r->setUniform("gb2_sampler", 2);
        r->setUniform("ambient_light", Vec3{0.02f, 0.02f, 0.02f});
        r->submit(0, post_process_);

        // Lights.
        for (int x = -3; x <= 3; x++) {
            for (int z = -3; z <= 3; z++) {
                point_lights.emplace_back(makeUnique<PointLight>(
                    context(), 10.0f,
                    Vec2{static_cast<float>(width()), static_cast<float>(height())}));
                point_lights.back()->setPosition(Vec3(x * 30.0f, -9.0f, z * 30.0f - 40.0f));
            }
        }
    }

    void render() override {
        // Set up views.
        r->setViewClear(0, {0.0f, 0.0f, 0.0f, 1.0f});
        r->setViewFrameBuffer(0, gbuffer_);
        r->setViewClear(1, {0.0f, 0.0f, 0.0f, 1.0f});
        r->setViewFrameBuffer(1, rhi::FrameBufferHandle{0});

        // Calculate matrices.
        Mat4 model = Mat4::Translate(Vec3{0.0f, -10.0f, 0.0f}).ToFloat4x4() *
                     Mat4::RotateX(math::pi * -0.5f);
        static Mat4 view = Mat4::Translate(Vec3{0.0f, 0.0f, 50.0f}).ToFloat4x4().Inverted();
        static Mat4 proj =
            util::createProjMatrix(0.1f, 1000.0f, 60.0f, static_cast<float>(width()) / height());
        r->setUniform("model_matrix", model);
        r->setUniform("mvp_matrix", proj * view * model);

        // Set vertex buffer and submit.
        r->setVertexBuffer(ground_->vertexBuffer()->internalHandle());
        r->setIndexBuffer(ground_->indexBuffer()->internalHandle());
        r->setTexture(texture_resource_->internalHandle(), 0);
        r->submit(0, cube_program_, ground_->indexBuffer()->indexCount());

        // Draw fb.
        r->setTexture(r->getFrameBufferTexture(gbuffer_, 0), 0);
        r->setTexture(r->getFrameBufferTexture(gbuffer_, 1), 1);
        r->setTexture(r->getFrameBufferTexture(gbuffer_, 2), 2);
        r->setVertexBuffer(fsq_vb_);
        r->submit(1, post_process_, 3);

        // Update and draw point lights.
        static float angle = 0.0f;
        angle += engine_->frameTime();
        int light_counter = 0;
        for (int x = -3; x <= 3; x++) {
            for (int z = -3; z <= 3; z++) {
                point_lights[light_counter]->setPosition(Vec3(
                    x * 20.0f + sin(angle) * 10.0f, -8.0f, z * 20.0f - 30.0f + cos(angle) * 10.0f));
                r->setTexture(r->getFrameBufferTexture(gbuffer_, 0), 0);
                r->setTexture(r->getFrameBufferTexture(gbuffer_, 1), 1);
                r->setTexture(r->getFrameBufferTexture(gbuffer_, 2), 2);
                point_lights[light_counter]->draw(1, view, proj);
                light_counter++;
            }
        }
    }

    void stop() override {
        r->deleteProgram(post_process_);
        r->deleteVertexBuffer(fsq_vb_);
        r->deleteProgram(cube_program_);
    }
};

TEST_CLASS(MovingSphere) {
    TEST_BODY(MovingSphere);

    Entity* object;
    Entity* camera;
    SystemNode* cube_node;

    void start() override {
        auto rc = module<ResourceCache>();
        auto scene = session_->sceneManager();
        auto scene_graph = session_->sceneGraph();

        // Set up resource cache.
        assert(rc);
        rc->addPath("base", "../media/base");
        rc->addPath("examples", "../media/examples");

        // Set up the environment.
        auto* frame = scene_graph->addFrame(&scene_graph->root());

        // Create an object.
        auto material = makeShared<Material>(
            context(),
            makeShared<Program>(context(), rc->get<VertexShader>("examples:shaders/cube_solid.vs").value(),
                                rc->get<FragmentShader>("examples:shaders/cube_solid.fs").value()));
        auto renderable = MeshBuilder(context()).normals(true).createSphere(10.0f);
        renderable->setMaterial(material);
        material->program()->setUniform("light_direction", Vec3{1.0f, 1.0f, 1.0f}.Normalized());

        object = &scene->createEntity(0, Vec3::zero, Quat::identity, *frame, renderable);
        auto outer_object =
            scene->createEntity(0, {20.0f, 0.0f, 0.0f}, Quat::identity, *object, renderable);
        outer_object.transform()->scale = Vec3(0.25f);

        // Create another object, not an entity, which is stored in a system node.
        auto cube = MeshBuilder(context()).normals(true).createBox(100.0f);
        cube->setMaterial(material);
        cube_node = scene_graph->root().newChild(SystemPosition{300.0f, 300.0f, -750.0f});
        cube_node->data.renderable = cube;

        // Create a camera.
        camera = &scene->createEntity(1, {0.0f, 0.0f, 60.0f}, Quat::identity, *frame);
        camera->addComponent<CCamera>(0.1f, 1000.0f, 60.0f, 1280.0f / 800.0f);
    }

    void stop() override {
        session_->sceneManager()->removeEntity(object);
        session_->sceneManager()->removeEntity(camera);
    }

    void render() override {
        static float angle = 0.0f;
        angle += engine_->frameTime();

        // Move some objects.
        camera->transform()->position.x = sin(angle) * 30.0f;
        object->transform()->orientation = Quat::RotateY(angle);
        cube_node->orientation = Quat::RotateX(angle) * Quat::RotateY(angle) * Quat::RotateZ(angle);

        r->setViewClear(0, {0.0f, 0.0f, 0.2f, 1.0f});
    }
};

TEST_CLASS(PostProcessInvert) {
    TEST_BODY(PostProcessInvert);

    Entity* object;
    Entity* camera;

    void start() override {
        using rds = RenderPipelineDesc;

        /*
        auto default_render_pipeline_node = RenderPipelineDesc::Node{
            {},
            {{"out", rhi::TextureFormat::RGBA8}},
            {RenderPipelineDesc::ClearStep{}, RenderPipelineDesc::RenderQueueStep{}} };
        auto default_render_pipeline =
            RenderPipelineDesc{ {{"Default", default_render_pipeline_node}},
                               {},
                               {RenderPipelineDesc::NodeInstance{
                                   "Default", {}, {{"out", RenderPipelineDesc::PipelineOutput}}}} };

        */

        auto render_scene = RenderPipelineDesc::Node{
            {},
            {{"out", rhi::TextureFormat::RGBA8}},
            {RenderPipelineDesc::ClearStep{}, RenderPipelineDesc::RenderQueueStep{}} };
        auto render_invert =
            rds::Node{ {{"in", rhi::TextureFormat::RGBA8}},
                      {{"out", rhi::TextureFormat::RGBA8}},
                      {rds::RenderQuadStep{"examples:custom/postprocess_inverse"}}};
        HashMap<String, rds::Node> nodes = { {"RenderScene", render_scene},
                                            {"RenderInvert", render_invert} };
        rds desc = { nodes,
                    {{"scene", rds::Texture{rhi::TextureFormat::RGBA8}}},
                    {rds::NodeInstance{"RenderScene", {}, {{"out", "scene"}}},
                     rds::NodeInstance{"RenderInvert",
                                       {{"in", "scene"}},
                                       {{"out", rds::PipelineOutput}}}} };

        auto rc = module<ResourceCache>();
        auto scene = session_->sceneManager();
        auto scene_graph = session_->sceneGraph();

        // Set up resource cache.
        assert(rc);
        rc->addPath("base", "../media/base");
        rc->addPath("examples", "../media/examples");

        // Create material for inverting the output.
        auto object_gbuffer = rc->addCustomResource(
            "examples:custom/postprocess_inverse",
            makeShared<Material>(
                context(),
                makeShared<Program>(
                    context(), rc->get<VertexShader>("examples:shaders/post_process.vs").value(),
                    rc->get<FragmentShader>("examples:shaders/post_process.fs").value())));
        object_gbuffer->setUniform("wall_sampler", 0);
        object_gbuffer->setUniform("texcoord_scale", Vec2{ 10.0f, 10.0f });

        // Set up render pipeline.
        session_->sceneGraph()->setRenderPipeline(RenderPipeline::createFromDesc(context(), desc).value());

        // Set up the environment.
        auto* frame = scene_graph->addFrame(&scene_graph->root());

        // Create an object.
        auto material = makeShared<Material>(
            context(),
            makeShared<Program>(context(), rc->get<VertexShader>("examples:shaders/cube_solid.vs").value(),
                rc->get<FragmentShader>("examples:shaders/cube_solid.fs").value()));
        auto renderable = MeshBuilder(context()).normals(true).createSphere(10.0f);
        renderable->setMaterial(material);
        material->program()->setUniform("light_direction", Vec3{ 1.0f, 1.0f, 1.0f }.Normalized());

        object = &scene->createEntity(0, Vec3::zero, Quat::identity, *frame, renderable);

        // Create a camera.
        camera = &scene->createEntity(1, { 0.0f, 0.0f, 60.0f }, Quat::identity, *frame);
        camera->addComponent<CCamera>(0.1f, 1000.0f, 60.0f, 1280.0f / 800.0f);
    }

    void render() override {
    }

    void stop() override {
        session_->sceneManager()->removeEntity(object);
        session_->sceneManager()->removeEntity(camera);
    }
};

TEST_CLASS(DeferredLighting) {
    TEST_BODY(DeferredLighting);

    SharedPtr<CustomMeshRenderable> ground_;

    void start() override {
        using rds = RenderPipelineDesc;
        auto generate_gbuffer = rds::Node{{},
                                          {{"gb0", rhi::TextureFormat::RGBA32F},
                                           {"gb1", rhi::TextureFormat::RGBA32F},
                                           {"gb2", rhi::TextureFormat::RGBA32F}},
                                          {rds::ClearStep{}, rds::RenderQueueStep{0x1}}};
        auto lighting =
            rds::Node{{{"gb0", rhi::TextureFormat::RGBA32F},
                       {"gb1", rhi::TextureFormat::RGBA32F},
                       {"gb2", rhi::TextureFormat::RGBA32F}},
                      {{"out", rhi::TextureFormat::RGBA8}},
                      {rds::RenderQuadStep{"examples:custom/deferred_ambient_light_pass"},
                       rds::RenderQueueStep{0x2}}};
        HashMap<String, rds::Node> nodes = {{"GenerateGBuffer", generate_gbuffer},
                                            {"Lighting", lighting}};
        rds desc = {nodes,
                    {{"gb0", rds::Texture{rhi::TextureFormat::RGBA32F}},
                     {"gb1", rds::Texture{rhi::TextureFormat::RGBA32F}},
                     {"gb2", rds::Texture{rhi::TextureFormat::RGBA32F}}},
                    {rds::NodeInstance{
                         "GenerateGBuffer", {}, {{"gb0", "gb0"}, {"gb1", "gb1"}, {"gb2", "gb2"}}},
                     rds::NodeInstance{"Lighting",
                                       {{"gb0", "gb0"}, {"gb1", "gb1"}, {"gb2", "gb2"}},
                                       {{"out", rds::PipelineOutput}}}}};

        auto rc = module<ResourceCache>();
        auto scene_graph = session_->sceneGraph();

        // Set up resource cache.
        assert(rc);
        rc->addPath("base", "../media/base");
        rc->addPath("examples", "../media/examples");

        // Create material for outputting object information to the gbuffer.
        auto object_gbuffer = rc->addCustomResource(
            "examples:custom/object_gbuffer",
            makeShared<Material>(
                context(),
                makeShared<Program>(
                    context(), rc->get<VertexShader>("examples:shaders/object_gbuffer.vs").value(),
                    rc->get<FragmentShader>("examples:shaders/object_gbuffer.fs").value())));
        object_gbuffer->setUniform("wall_sampler", 0);
        object_gbuffer->setUniform("texcoord_scale", Vec2{10.0f, 10.0f});

        // Create material for processing the ambient light in the deferred shading.
        auto ambient_light_pass = rc->addCustomResource(
            "examples:custom/deferred_ambient_light_pass",
            makeShared<Material>(
                context(),
                makeShared<Program>(
                    context(), rc->get<VertexShader>("examples:shaders/post_process.vs").value(),
                    rc->get<FragmentShader>("examples:shaders/deferred_ambient_light_pass.fs").value())));
        ambient_light_pass->setUniform("ambient_light", Vec3{0.02f, 0.02f, 0.02f});

        // Create ground.
        ground_ = MeshBuilder{context_}.normals(true).texcoords(true).createPlane(250.0f, 250.0f);

        /*
        // Load shaders.
        auto vs =
            util::loadShader(context(), rhi::ShaderStage::Vertex, "shaders/object_gbuffer.vs");
        auto fs =
            util::loadShader(context(), rhi::ShaderStage::Fragment, "shaders/object_gbuffer.fs");
        cube_program_ = r->createProgram();
        r->attachShader(cube_program_, vs);
        r->attachShader(cube_program_, fs);
        r->linkProgram(cube_program_);
        r->setUniform("wall_sampler", 0);
        r->setUniform("texcoord_scale", Vec2{ 10.0f, 10.0f });
        r->submit(0, cube_program_);

        // Create ground.
        ground_ = MeshBuilder{ context_ }.normals(true).texcoords(true).createPlane(250.0f, 250.0f);

        // Load texture.
        File texture_file{ context(), "wall.jpg" };
        texture_resource_ = makeUnique<Texture>(context());
        texture_resource_->beginLoad("wall.jpg", texture_file);
        texture_resource_->endLoad();

        // Create full screen quad.
        util::createFullscreenQuad(r, fsq_vb_);

        // Set up frame buffer.
        auto format = rhi::TextureFormat::RGBA32F;
        gbuffer_ = r->createFrameBuffer({ r->createTexture2D(width(), height(), format, Memory()),
                                         r->createTexture2D(width(), height(), format, Memory()),
                                         r->createTexture2D(width(), height(), format, Memory()) });

        // Load post process shader.
        auto pp_vs =
            util::loadShader(context(), rhi::ShaderStage::Vertex, "shaders/post_process.vs");
        auto pp_fs = util::loadShader(context(), rhi::ShaderStage::Fragment,
            "shaders/deferred_ambient_light_pass.fs");
        post_process_ = r->createProgram();
        r->attachShader(post_process_, pp_vs);
        r->attachShader(post_process_, pp_fs);
        r->linkProgram(post_process_);
        r->setUniform("gb0_sampler", 0);
        r->setUniform("gb1_sampler", 1);
        r->setUniform("gb2_sampler", 2);
        r->setUniform("ambient_light", Vec3{ 0.02f, 0.02f, 0.02f });
        r->submit(0, post_process_);
        */
    }

    void render() override {
    }

    void stop() override {
    }
};

using ExampleApp =
    ExampleAppContainer<RHIBasicVertexBuffer, RHIBasicIndexBuffer, RHITransientIndexBuffer,
                        RHITextured3DCube, RHIPostProcessing, RHIDeferredShading, MovingSphere,
    PostProcessInvert, DeferredLighting>;
DW_IMPLEMENT_MAIN(ExampleApp)
