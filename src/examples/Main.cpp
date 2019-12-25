/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#include "Core.h"
#include "Renderer.h"
#include "Scene.h"
#include "resource/ResourceCache.h"

using namespace dw;

class Example : public Object {
public:
    DW_OBJECT(Example);

    Example(Context* ctx, const GameSession* session)
        : Object(ctx), r{ctx->module<Renderer>()->gfx()}, session_{session} {
    }

    u16 width() const {
        return context()->config().at("window_width").get<u16>();
    }

    u16 height() const {
        return context()->config().at("window_height").get<u16>();
    }

    virtual void start() {
    }
    virtual void render(float dt) {
    }
    virtual void stop() {
    }

    gfx::Renderer* r;
    const GameSession* session_;
};

template <typename... T> class ExampleAppContainer : public App {
public:
    DW_OBJECT(ExampleAppContainer<T...>);

    SessionId example_session;

    class ExampleGameSession : public GameSession {
    public:
        Vector<SharedPtr<Example>> examples_;
        Example* current_example_;
        int frame_id_;
        int frame_count_;
        int last_fps_;
        float accumulated_time_;

        ExampleGameSession(Context* ctx)
            : GameSession(ctx, GameSessionInfo{}),
              current_example_(nullptr),
              frame_id_(0),
              frame_count_(0),
              last_fps_(0),
              accumulated_time_(0.0f) {
            examples_ = {makeShared<T>(context_, this)...};
        }

        ~ExampleGameSession() {
            if (current_example_) {
                current_example_->stop();
                current_example_ = nullptr;
            }
            examples_.clear();
        }

        void render(float dt, float interpolation) override {
            GameSession::render(dt, interpolation);

            // Store FPS history.
            float current_fps = module<Renderer>()->framesPerSecond();
            static const int FPS_HISTORY_COUNT = 100;
            static float fps_history[FPS_HISTORY_COUNT];
            static double accumulated_time = 0.0;
            accumulated_time += dt;
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
            ImGui::PlotLines("", fps_history, FPS_HISTORY_COUNT, 0, nullptr, 0.0f, 1000.0f,
                             {250, 80});
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
                current_example_->render(dt);
                frame_id_++;
            }
        }
    };

    ExampleAppContainer() : App("Examples Viewer", DW_VERSION_STR), example_session(0) {
    }

    void init(const CommandLine&) override {
        example_session = engine_->addSession(makeUnique<ExampleGameSession>(context()));
    }

    void shutdown() override {
        engine_->removeSession(example_session);
    }
};

#define TEST_CLASS(test_name) class test_name : public Example
#define TEST_BODY(test_name)                                                      \
public:                                                                           \
    DW_OBJECT(test_name)                                                          \
    test_name(Context* ctx, const GameSession* session) : Example(ctx, session) { \
    }

// Utils
namespace util {
Mat4 createProjMatrix(float n, float f, float fov_y, float aspect) {
    float tangent = static_cast<float>(tan(fov_y * dw::M_DEGTORAD_OVER_2));  // tangent of half fovY
    float v = n * tangent * 2;  // half height of near plane
    float h = v * aspect;       // half width of near plane
    return Mat4::OpenGLPerspProjRH(n, f, h, v);
}

uint createFullscreenQuad(gfx::Renderer* r, gfx::VertexBufferHandle& vb) {
    // clang-format off
    float vertices[] = {
    	// Position   | UV
        -1.0f, -1.0f, 0.0f, 0.0f,
    	3.0f,  -1.0f, 2.0f, 0.0f,
    	-1.0f,  3.0f, 0.0f, 2.0f};
    // clang-format on
    gfx::VertexDecl decl;
    decl.begin()
        .add(gfx::VertexDecl::Attribute::Position, 2, gfx::VertexDecl::AttributeType::Float)
        .add(gfx::VertexDecl::Attribute::TexCoord0, 2, gfx::VertexDecl::AttributeType::Float)
        .end();
    vb = r->createVertexBuffer(gfx::Memory(vertices, sizeof(vertices)), decl);
    return 3;
}

gfx::ShaderHandle loadShader(Context* ctx, gfx::ShaderStage type, const String& source_file) {
    static Vector<SharedPtr<Shader>> shader_map;
    SharedPtr<Shader> shader = makeShared<Shader>(ctx, type);
    File file{ctx, source_file};
    shader->beginLoad(source_file, file);
    shader->endLoad();
    shader_map.emplace_back(shader);
    return shader->internalHandle();
}
}  // namespace util

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
            makeShared<Program>(context(), *rc->get<VertexShader>("examples:shaders/cube_solid.vs"),
                                *rc->get<FragmentShader>("examples:shaders/cube_solid.fs")));
        auto renderable = CustomRenderable::Builder(context()).normals(true).createSphere(10.0f);
        renderable->setMaterial(material);
        material->program()->setUniform("light_direction", Vec3{1.0f, 1.0f, 1.0f}.Normalized());

        object = &scene->createEntity(0, Vec3::zero, Quat::identity, *frame, renderable);
        auto outer_object =
            scene->createEntity(0, {20.0f, 0.0f, 0.0f}, Quat::identity, *object, renderable);
        outer_object.transform()->scale = Vec3(0.25f);

        // Create another object, not an entity, which is stored in a system node.
        auto cube = CustomRenderable::Builder(context()).normals(true).createBox(100.0f);
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

    void render(float dt) override {
        static float angle = 0.0f;
        angle += dt;

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
        auto render_scene = RenderPipelineDesc::Node{
            {},
            {{"out", gfx::TextureFormat::RGBA8}},
            {RenderPipelineDesc::ClearStep{}, RenderPipelineDesc::RenderQueueStep{}}};
        auto render_invert =
            rds::Node{{{"in", gfx::TextureFormat::RGBA8}},
                      {{"out", gfx::TextureFormat::RGBA8}},
                      {rds::RenderQuadStep{"examples:custom/postprocess_inverse"}}};
        HashMap<String, rds::Node> nodes = {{"RenderScene", render_scene},
                                            {"RenderInvert", render_invert}};
        rds desc = {
            nodes,
            {{"scene", rds::Texture{gfx::TextureFormat::RGBA8}}},
            {rds::NodeInstance{"RenderScene", {}, {{"out", "scene"}}},
             rds::NodeInstance{"RenderInvert", {{"in", "scene"}}, {{"out", rds::PipelineOutput}}}}};

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
                makeShared<Program>(context(),
                                    *rc->get<VertexShader>("examples:shaders/post_process.vs"),
                                    *rc->get<FragmentShader>("examples:shaders/post_process.fs"))));
        object_gbuffer->setUniform("wall_sampler", 0);
        object_gbuffer->setUniform("texcoord_scale", Vec2{10.0f, 10.0f});

        // Set up render pipeline.
        session_->sceneGraph()->setRenderPipeline(
            RenderPipeline::createFromDesc(context(), desc).value());

        // Set up the environment.
        auto* frame = scene_graph->addFrame(&scene_graph->root());

        // Create an object.
        auto material = makeShared<Material>(
            context(),
            makeShared<Program>(context(), *rc->get<VertexShader>("examples:shaders/cube_solid.vs"),
                                *rc->get<FragmentShader>("examples:shaders/cube_solid.fs")));
        auto renderable = CustomRenderable::Builder(context()).normals(true).createSphere(10.0f);
        renderable->setMaterial(material);
        material->program()->setUniform("light_direction", Vec3{1.0f, 1.0f, 1.0f}.Normalized());

        object = &scene->createEntity(0, Vec3::zero, Quat::identity, *frame, renderable);

        // Create a camera.
        camera = &scene->createEntity(1, {0.0f, 0.0f, 60.0f}, Quat::identity, *frame);
        camera->addComponent<CCamera>(0.1f, 1000.0f, 60.0f, 1280.0f / 800.0f);
    }

    void stop() override {
        session_->sceneManager()->removeEntity(object);
        session_->sceneManager()->removeEntity(camera);
    }
};

TEST_CLASS(DeferredLighting) {
    TEST_BODY(DeferredLighting);

    SharedPtr<RenderPipeline> deferred_lighting_pipeline;
    SharedPtr<RenderPipeline> deferred_debug_pipeline;

    Entity* ground;
    Vector<Entity*> lights;
    Entity* camera;

    const u32 kDiffuseMask = 0x1;
    const u32 kLightMask = 0x2;

    void start() override {
        using rds = RenderPipelineDesc;
        auto generate_gbuffer = rds::Node{{},
                                          {{"gb0", gfx::TextureFormat::RGBA32F},
                                           {"gb1", gfx::TextureFormat::RGBA32F},
                                           {"gb2", gfx::TextureFormat::RGBA32F}},
                                          {rds::ClearStep{}, rds::RenderQueueStep{kDiffuseMask}}};
        auto lighting =
            rds::Node{{{"gb0", gfx::TextureFormat::RGBA32F},
                       {"gb1", gfx::TextureFormat::RGBA32F},
                       {"gb2", gfx::TextureFormat::RGBA32F}},
                      {{"out", gfx::TextureFormat::RGBA8}},
                      {rds::RenderQuadStep{"examples:custom/deferred_ambient_light_pass"},
                       rds::RenderQueueStep{kLightMask}}};
        HashMap<String, rds::Node> nodes = {{"GenerateGBuffer", generate_gbuffer},
                                            {"Lighting", lighting}};
        auto debug_gbuffer =
            rds::Node{{{"gb0", gfx::TextureFormat::RGBA32F},
                       {"gb1", gfx::TextureFormat::RGBA32F},
                       {"gb2", gfx::TextureFormat::RGBA32F}},
                      {{"out", gfx::TextureFormat::RGBA8}},
                      {rds::RenderQuadStep{"examples:custom/deferred_debug_gbuffer"}}};

        rds deferred_lighting_desc = {
            {{"GenerateGBuffer", generate_gbuffer}, {"Lighting", lighting}},
            {{"gb0", rds::Texture{gfx::TextureFormat::RGBA32F}},
             {"gb1", rds::Texture{gfx::TextureFormat::RGBA32F}},
             {"gb2", rds::Texture{gfx::TextureFormat::RGBA32F}}},
            {rds::NodeInstance{
                 "GenerateGBuffer", {}, {{"gb0", "gb0"}, {"gb1", "gb1"}, {"gb2", "gb2"}}},
             rds::NodeInstance{"Lighting",
                               {{"gb0", "gb0"}, {"gb1", "gb1"}, {"gb2", "gb2"}},
                               {{"out", rds::PipelineOutput}}}}};

        rds deferred_debug_desc = {
            {{"GenerateGBuffer", generate_gbuffer}, {"DebugGBuffer", debug_gbuffer}},
            {{"gb0", rds::Texture{gfx::TextureFormat::RGBA32F}},
             {"gb1", rds::Texture{gfx::TextureFormat::RGBA32F}},
             {"gb2", rds::Texture{gfx::TextureFormat::RGBA32F}}},
            {rds::NodeInstance{
                 "GenerateGBuffer", {}, {{"gb0", "gb0"}, {"gb1", "gb1"}, {"gb2", "gb2"}}},
             rds::NodeInstance{"DebugGBuffer",
                               {{"gb0", "gb0"}, {"gb1", "gb1"}, {"gb2", "gb2"}},
                               {{"out", rds::PipelineOutput}}}}};

        auto rc = module<ResourceCache>();
        auto scene = session_->sceneManager();
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
                    context(), *rc->get<VertexShader>("examples:shaders/object_gbuffer.vs"),
                    *rc->get<FragmentShader>("examples:shaders/object_gbuffer.fs"))));
        object_gbuffer->setUniform("wall_sampler", 0);
        object_gbuffer->setUniform("texcoord_scale", Vec2{10.0f, 10.0f});

        // Create material for processing the ambient light in the deferred shading.
        auto ambient_light_pass_material = rc->addCustomResource(
            "examples:custom/deferred_ambient_light_pass",
            makeShared<Material>(
                context(),
                makeShared<Program>(
                    context(), *rc->get<VertexShader>("examples:shaders/post_process.vs"),
                    *rc->get<FragmentShader>("examples:shaders/deferred_ambient_light_pass.fs"))));
        ambient_light_pass_material->setUniform("ambient_light", Vec3{0.05f, 0.05f, 0.05f});

        // Create material for displaying the gbuffer in debug mode.
        auto debug_gbuffer_material = rc->addCustomResource(
            "examples:custom/deferred_debug_gbuffer",
            makeShared<Material>(
                context(),
                makeShared<Program>(
                    context(), *rc->get<VertexShader>("examples:shaders/post_process.vs"),
                    *rc->get<FragmentShader>("examples:shaders/deferred_debug_gbuffer.fs"))));

        // Create render pipelines.
        deferred_lighting_pipeline =
            *RenderPipeline::createFromDesc(context(), deferred_lighting_desc);
        deferred_debug_pipeline = *RenderPipeline::createFromDesc(context(), deferred_debug_desc);
        session_->sceneGraph()->setRenderPipeline(deferred_lighting_pipeline);

        //////////////////

        // Set up the environment.
        auto* frame = scene_graph->addFrame(&scene_graph->root());

        // Create ground.
        auto ground_material = makeShared<Material>(
            context(), makeShared<Program>(
                           context(), *rc->get<VertexShader>("examples:shaders/object_gbuffer.vs"),
                           *rc->get<FragmentShader>("examples:shaders/object_gbuffer.fs")));
        ground_material->setTexture(*rc->get<Texture>("examples:wall.jpg"), 0);
        ground_material->setUniform("wall_sampler", 0);
        ground_material->setUniform("texcoord_scale", Vec2{10.0f, 10.0f});
        auto ground_renderable =
            CustomRenderable::Builder{context_}.normals(true).texcoords(true).createPlane(250.0f,
                                                                                          250.0f);
        ground_renderable->setMaterial(ground_material);
        ground = &scene->createEntity(0, Vec3{0.0f, 0.0f, 0.0f}, Quat::RotateX(math::pi * -0.5f),
                                      *frame, ground_renderable);

        // Create lights.
        const float light_radius = 60.0f;
        const float light_mesh_radius = light_radius * 3.0f;
        auto light_material = makeShared<Material>(
            context(),
            makeShared<Program>(context(), *rc->get<VertexShader>("examples:shaders/light_pass.vs"),
                                *rc->get<FragmentShader>("examples:shaders/point_light_pass.fs")));
        light_material->setUniform("screen_size",
                                   Vec2(r->backbufferSize().x, r->backbufferSize().y));
        light_material->setUniform("radius", light_radius);
        light_material->setUniform<int>("gb0_sampler", 0);
        light_material->setUniform<int>("gb1_sampler", 1);
        light_material->setUniform<int>("gb2_sampler", 2);
        light_material->setTexture(deferred_lighting_pipeline->texture("gb0"), 0);
        light_material->setTexture(deferred_lighting_pipeline->texture("gb1"), 1);
        light_material->setTexture(deferred_lighting_pipeline->texture("gb2"), 2);
        light_material->setDepthWrite(false);
        light_material->setStateDisable(gfx::RenderState::Depth);
        light_material->setStateEnable(gfx::RenderState::Blending);
        light_material->setBlendEquation(gfx::BlendEquation::Add, gfx::BlendFunc::One,
                                         gfx::BlendFunc::One);
        light_material->setMask(kLightMask);

        for (int x = -2; x <= 2; ++x) {
            for (int z = -2; z <= 2; ++z) {
                auto light_renderable = CustomRenderable::Builder{context_}
                                            .normals(false)
                                            .texcoords(false)
                                            .createSphere(light_mesh_radius, 8, 8);
                light_renderable->setMaterial(makeShared<Material>(*light_material));
                lights.emplace_back(&scene->createEntity(0,
                                                         Vec3(x * 40.0f, 1.0f, z * 40.0f - 40.0f),
                                                         Quat::identity, *frame, light_renderable));
            }
        }

        // Create a camera.
        camera = &scene->createEntity(1, {0.0f, 10.0f, 60.0f}, Quat::identity, *frame);
        camera->addComponent<CCamera>(0.1f, 1000.0f, 60.0f, 1280.0f / 800.0f);

        // Set up render camera callback.
        scene_graph->preRenderCameraCallback = [this, light_mesh_radius](
                                                   float dt,
                                                   const detail::Transform& camera_transform,
                                                   const Mat4&, const Mat4&) {
            // Update point lights.
            static float angle = 0.0f;
            angle += dt;
            int light_counter = 0;
            for (int x = -2; x <= 2; x++) {
                for (int z = -2; z <= 2; z++) {
                    auto light_position = Vec3(x * 30.0f + sin(angle) * 10.0f, 4.0f,
                                               z * 30.0f - 30.0f + cos(angle) * 10.0f);
                    auto* material =
                        lights[light_counter]->component<CSceneNode>()->renderable()->material();
                    lights[light_counter]->transform()->position = light_position;
                    light_counter++;

                    // Update material.
                    material->setUniform("light_position", light_position);
                    bool should_invert_sphere =
                        camera_transform.position.DistanceSq(light_position) <
                        light_mesh_radius * light_mesh_radius;
                    material->setCullFrontFace(should_invert_sphere ? gfx::CullFrontFace::CW
                                                                    : gfx::CullFrontFace::CCW);
                }
            }
        };
    }

    void stop() override {
    }
};

using ExampleApp = ExampleAppContainer<MovingSphere, PostProcessInvert, DeferredLighting>;
DW_IMPLEMENT_MAIN(ExampleApp)
