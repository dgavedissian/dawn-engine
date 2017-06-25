/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "DawnEngine.h"

using namespace dw;

class RendererTest1 : public App {
public:
    DW_OBJECT(RendererTest1);

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


class RendererTest2 : public App {
public:
    DW_OBJECT(RendererTest2);

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
                -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Top-left
                0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Top-right
                0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right
                -0.5f, -0.5f, 1.0f, 1.0f, 1.0f  // Bottom-left
        };
        VertexDecl decl;
        decl.begin()
                .add(VertexDecl::Attribute::Position, 2, VertexDecl::AttributeType::Float)
                .add(VertexDecl::Attribute::Colour, 3, VertexDecl::AttributeType::Float)
                .end();
        vb_ = r->createVertexBuffer(vertices, sizeof(vertices), decl);

        GLuint elements[] = {
                0, 1, 2,
                2, 3, 0
        };
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

DW_IMPLEMENT_MAIN(RendererTest2);
