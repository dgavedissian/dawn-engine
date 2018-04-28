/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "core/io/InputStream.h"
#include "renderer/Mesh.h"
#include "core/StringUtils.h"

#define ASSIMP_BUILD_BOOST_WORKAROUND
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/Logger.hpp>
#include <assimp/postprocess.h>

namespace dw {
namespace {
class DawnAssimpLogStream : public Assimp::LogStream {
public:
    DawnAssimpLogStream(Context* ctx) : logger{ctx->module<Logger>()} {
    }
    ~DawnAssimpLogStream() = default;

    void write(const char* message_cstr) {
        String message{message_cstr};
        logger->withObjectName("Mesh").info("Assimp Importer: %s",
                                            message.substr(0, message.length() - 1));
    }

private:
    Logger* logger;
};
}  // namespace

Mesh::Mesh(Context* context) : Resource(context) {
}

Mesh::~Mesh() {
}

bool Mesh::beginLoad(const String& asset_name, InputStream& is) {
    // Read stream.
    assert(is.size() > 0);
    u64 size = is.size();
    byte* data = new byte[size];
    is.readData(data, size);
    assert(is.eof());

    const unsigned int severity = Assimp::Logger::Debugging | Assimp::Logger::Info |
                                  Assimp::Logger::Warn | Assimp::Logger::Err;

    // Run importer.
    Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE, 0);
    Assimp::DefaultLogger::get()->attachStream(new DawnAssimpLogStream(context()), severity);
    Assimp::Importer importer;
    auto flags = aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                 aiProcess_JoinIdenticalVertices | aiProcess_SortByPType;
    const aiScene* scene = importer.ReadFileFromMemory(data, size, flags, asset_name.c_str());
    Assimp::DefaultLogger::kill();
    if (scene == nullptr) {
        log().error("Unable to load mesh. Reason: %s", importer.GetErrorString());
        return false;
    }

    // Parse scene returned by Assimp.
    struct Vertex {
        Vec3 position;
        Vec3 normal;
    };
    Vector<Vertex> vertices;
    Vector<u32> indices;
    uint vertex_offset = 0;
    for (uint i = 0; i < scene->mNumMeshes; ++i) {
        auto mesh = scene->mMeshes[i];
        if (!mesh->HasPositions()) {
            log().warn("Mesh importer: Skipping mesh %d as it has no positions.", i);
            continue;
        }
        if (!mesh->HasNormals()) {
            log().warn("Mesh importer: Skipping mesh %d as it has no normals.", i);
            continue;
        }
        if (!mesh->HasFaces()) {
            log().warn("Mesh importer: Skipping mesh %d as it has no faces.", i);
            continue;
        }
        for (uint v = 0; v < mesh->mNumVertices; ++v) {
            aiVector3D& position = mesh->mVertices[v];
            aiVector3D& normal = mesh->mNormals[v];
            vertices.emplace_back(
                Vertex{{position.x, position.y, position.z}, {normal.x, normal.y, normal.z}});
        }
        for (uint f = 0; f < mesh->mNumFaces; ++f) {
            aiFace& face = mesh->mFaces[f];
            if (face.mNumIndices != 3) {
                log().warn(
                    "Mesh importer: Skipping primitive %d in mesh %d, as it doesn't have 3 "
                    "indices.",
                    f, i);
                continue;
            }
            indices.emplace_back(face.mIndices[0] + vertex_offset);
            indices.emplace_back(face.mIndices[1] + vertex_offset);
            indices.emplace_back(face.mIndices[2] + vertex_offset);
        }
        vertex_offset += mesh->mNumVertices;
    }

    // Build GPU buffers.
    rhi::VertexDecl decl;
    decl.begin()
        .add(rhi::VertexDecl::Attribute::Position, 3, rhi::VertexDecl::AttributeType::Float)
        .add(rhi::VertexDecl::Attribute::Normal, 3, rhi::VertexDecl::AttributeType::Float)
        .end();
    vertex_buffer_ = makeShared<VertexBuffer>(
        context(), vertices.data(), vertices.size() * sizeof(Vertex), vertices.size(), decl);
    index_buffer_ = makeShared<IndexBuffer>(context(), indices.data(), indices.size(),
                                            rhi::IndexBufferType::U32);
    return true;
}

void Mesh::endLoad() {
}

void Mesh::draw(Renderer* renderer, uint view, Transform* camera, const Mat4& model_matrix,
                const Mat4& view_projection_matrix) {
    u32 vertex_count = index_buffer_->indexCount();
    auto rhi = renderer->rhi();
    rhi->setVertexBuffer(vertex_buffer_->internalHandle());
    rhi->setIndexBuffer(index_buffer_->internalHandle());
    // TODO: Move this common "render vertex/index buffer + material" code somewhere to avoid
    // duplication with CustomMeshRenderable.
    // TODO: Support unset material.
    material_->applyRendererState(model_matrix, view_projection_matrix);
    rhi->submit(view, material_->program()->internalHandle(), vertex_count);
}
}  // namespace dw
