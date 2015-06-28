/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#include "Common.h"

#define B SAMPLE_SIZE
#define BM (SAMPLE_SIZE - 1)

#define N 0x1000
#define NP 12 /* 2^N */
#define NM 0xfff

#define s_curve(t) (t * t * (3.0f - 2.0f * t))
#define setup(i, b0, b1, r0, r1) \
    t = vec[i] + N;              \
    b0 = ((int)t) & BM;          \
    b1 = (b0 + 1) & BM;          \
    r0 = t - (int)t;             \
    r1 = r0 - 1.0f;

NAMESPACE_BEGIN

PerlinNoise::PerlinNoise(int octaves, float freq, float amp, int seed)
{
    mOctaves = octaves;
    mFrequency = freq;
    mAmplitude = amp;
    mSeed = seed;
    mStart = true;
}

float PerlinNoise::noise(float x)
{
    return perlinNoise1(x);
}

float PerlinNoise::noise(float x, float y)
{
    float vec[2];
    vec[0] = x;
    vec[1] = y;
    return perlinNoise2(vec);
};

float PerlinNoise::noise(float x, float y, float z)
{
    float vec[3];
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
    return perlinNoise3(vec);
}

void PerlinNoise::init()
{
    int i, j, k;

    for (i = 0; i < B; i++)
    {
        p[i] = i;
        g1[i] = (float)((rand() % (B + B)) - B) / B;

        for (j = 0; j < 2; j++)
        {
            g2[i][j] = (float)((rand() % (B + B)) - B) / B;
        }

        normalise2(g2[i]);

        for (j = 0; j < 3; j++)
        {
            g3[i][j] = (float)((rand() % (B + B)) - B) / B;
        }

        normalise3(g3[i]);
    }

    while (--i)
    {
        k = p[i];
        p[i] = p[j = rand() % B];
        p[j] = k;
    }

    for (i = 0; i < B + 2; i++)
    {
        p[B + i] = p[i];
        g1[B + i] = g1[i];

        for (j = 0; j < 2; j++)
        {
            g2[B + i][j] = g2[i][j];
        }

        for (j = 0; j < 3; j++)
        {
            g3[B + i][j] = g3[i][j];
        }
    }
}

float PerlinNoise::perlinNoise1(float arg)
{
    int terms = mOctaves;
    float result = 0.0f;
    float amp = mAmplitude;

    arg *= mFrequency;

    for (int i = 0; i < terms; i++)
    {
        result += noise1(arg) * amp;
        arg *= 2.0f;
        amp *= 0.5f;
    }

    return result;
}

float PerlinNoise::perlinNoise2(float vec[2])
{
    int terms = mOctaves;
    float result = 0.0f;
    float amp = mAmplitude;

    vec[0] *= mFrequency;
    vec[1] *= mFrequency;

    for (int i = 0; i < terms; i++)
    {
        result += noise2(vec) * amp;
        vec[0] *= 2.0f;
        vec[1] *= 2.0f;
        amp *= 0.5f;
    }

    return result;
}

float PerlinNoise::perlinNoise3(float vec[3])
{
    int terms = mOctaves;
    float result = 0.0f;
    float amp = mAmplitude;

    vec[0] *= mFrequency;
    vec[1] *= mFrequency;
    vec[2] *= mFrequency;

    for (int i = 0; i < terms; i++)
    {
        result += noise3(vec) * amp;
        vec[0] *= 2.0f;
        vec[1] *= 2.0f;
        vec[2] *= 2.0f;
        amp *= 0.5f;
    }

    return result;
}

float PerlinNoise::noise1(float arg)
{
    int bx0, bx1;
    float rx0, rx1, sx, t, u, v, vec[1];

    vec[0] = arg;

    if (mStart)
    {
        srand(mSeed);
        mStart = false;
        init();
    }

    setup(0, bx0, bx1, rx0, rx1);

    sx = s_curve(rx0);

    u = rx0 * g1[p[bx0]];
    v = rx1 * g1[p[bx1]];

    return math::Lerp(sx, u, v);
}

float PerlinNoise::noise2(float vec[2])
{
    int bx0, bx1, by0, by1, b00, b10, b01, b11;
    float rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
    int i, j;

    if (mStart)
    {
        srand(mSeed);
        mStart = false;
        init();
    }

    setup(0, bx0, bx1, rx0, rx1);
    setup(1, by0, by1, ry0, ry1);

    i = p[bx0];
    j = p[bx1];

    b00 = p[i + by0];
    b10 = p[j + by0];
    b01 = p[i + by1];
    b11 = p[j + by1];

    sx = s_curve(rx0);
    sy = s_curve(ry0);

#define at2(rx, ry) (rx * q[0] + ry * q[1])

    q = g2[b00];
    u = at2(rx0, ry0);
    q = g2[b10];
    v = at2(rx1, ry0);
    a = math::Lerp(sx, u, v);

    q = g2[b01];
    u = at2(rx0, ry1);
    q = g2[b11];
    v = at2(rx1, ry1);
    b = math::Lerp(sx, u, v);

    return math::Lerp(sy, a, b);
}

float PerlinNoise::noise3(float vec[3])
{
    int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
    float rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
    int i, j;

    if (mStart)
    {
        srand(mSeed);
        mStart = false;
        init();
    }

    setup(0, bx0, bx1, rx0, rx1);
    setup(1, by0, by1, ry0, ry1);
    setup(2, bz0, bz1, rz0, rz1);

    i = p[bx0];
    j = p[bx1];

    b00 = p[i + by0];
    b10 = p[j + by0];
    b01 = p[i + by1];
    b11 = p[j + by1];

    t = s_curve(rx0);
    sy = s_curve(ry0);
    sz = s_curve(rz0);

#define at3(rx, ry, rz) (rx * q[0] + ry * q[1] + rz * q[2])

    q = g3[b00 + bz0];
    u = at3(rx0, ry0, rz0);
    q = g3[b10 + bz0];
    v = at3(rx1, ry0, rz0);
    a = math::Lerp(t, u, v);

    q = g3[b01 + bz0];
    u = at3(rx0, ry1, rz0);
    q = g3[b11 + bz0];
    v = at3(rx1, ry1, rz0);
    b = math::Lerp(t, u, v);

    c = math::Lerp(sy, a, b);

    q = g3[b00 + bz1];
    u = at3(rx0, ry0, rz1);
    q = g3[b10 + bz1];
    v = at3(rx1, ry0, rz1);
    a = math::Lerp(t, u, v);

    q = g3[b01 + bz1];
    u = at3(rx0, ry1, rz1);
    q = g3[b11 + bz1];
    v = at3(rx1, ry1, rz1);
    b = math::Lerp(t, u, v);

    d = math::Lerp(sy, a, b);

    return math::Lerp(sz, c, d);
}

void PerlinNoise::normalise2(float v[2])
{
    float s;

    s = (float)sqrt(v[0] * v[0] + v[1] * v[1]);
    s = 1.0f / s;
    v[0] = v[0] * s;
    v[1] = v[1] * s;
}

void PerlinNoise::normalise3(float v[3])
{
    float s;

    s = (float)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    s = 1.0f / s;

    v[0] = v[0] * s;
    v[1] = v[1] * s;
    v[2] = v[2] * s;
}

CommandLineArgs ParseCommandArgs(int argc, char** argv)
{
    CommandLineArgs args;

    // Parse arguments
    auto dispatchArgument = [&args](const string& name, const string& param)
    {
        if (name.length() > 0)
        {
            LOG << "\tName: " << name << " - Param: " << param;
            args.insert(make_pair(name, param));
        }
    };

    // Walk through argc/argv
    LOG << "Program Arguments:";
    string name, param;
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            dispatchArgument(name, param);
            name = string(argv[i]).substr(1);
            param.clear();
        }
        else
        {
            if (!param.empty())
                LOG << "Extra argument specified, ignoring";
            else
                param = string(argv[i]);
        }
    }

    // Dispatch final parameter
    dispatchArgument(name, param);

    return args;
}

Ogre::MeshPtr CreateSphere(const string& name, float radius, int rings, int segments, bool normals,
                           bool texCoords)
{
    Ogre::MeshPtr sphere = Ogre::MeshManager::getSingleton().createManual(
        name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Ogre::SubMesh* sphereVertex = sphere->createSubMesh();
    sphere->sharedVertexData = new Ogre::VertexData();
    CreateSphere(sphere->sharedVertexData, sphereVertex->indexData, radius, rings, segments,
                 normals, texCoords);

    // Set up mesh
    sphereVertex->useSharedVertices = true;
    sphere->_setBounds(Ogre::AxisAlignedBox(Ogre::Vector3(-radius, -radius, -radius),
                                            Ogre::Vector3(radius, radius, radius)),
                       false);
    sphere->_setBoundingSphereRadius(radius);
    sphere->load();
    return sphere;
}

void CreateSphere(Ogre::VertexData* vertexData, Ogre::IndexData* indexData, float radius, int rings,
                  int segments, bool normals, bool texCoords)
{
    assert(vertexData && indexData);

    // Define the vertex format
    Ogre::VertexDeclaration* vertexDecl = vertexData->vertexDeclaration;
    size_t currOffset = 0;

    // Position
    vertexDecl->addElement(0, currOffset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
    currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

    // Normals
    if (normals)
    {
        vertexDecl->addElement(0, currOffset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
        currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
    }

    // Texture coordinates
    if (texCoords)
    {
        vertexDecl->addElement(0, currOffset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 0);
    }

    // Allocate the vertex buffer
    vertexData->vertexCount = (rings + 1) * (segments + 1);
    Ogre::HardwareVertexBufferSharedPtr vBuf =
        Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
            vertexDecl->getVertexSize(0), vertexData->vertexCount,
            Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
    Ogre::VertexBufferBinding* binding = vertexData->vertexBufferBinding;
    binding->setBinding(0, vBuf);
    float* vertex = static_cast<float*>(vBuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));

    // Allocate index buffer
    indexData->indexCount = 6 * rings * (segments + 1);
    indexData->indexBuffer = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
        Ogre::HardwareIndexBuffer::IT_16BIT, indexData->indexCount,
        Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
    Ogre::HardwareIndexBufferSharedPtr iBuf = indexData->indexBuffer;
    unsigned short* indices =
        static_cast<unsigned short*>(iBuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));

    float deltaRingAngle = math::pi / rings;
    float deltaSegAngle = 2.0f * math::pi / segments;
    unsigned short wVerticeIndex = 0;

    // Generate the group of rings for the sphere
    for (int ring = 0; ring <= rings; ring++)
    {
        float r0 = radius * sinf(ring * deltaRingAngle);
        float y0 = radius * cosf(ring * deltaRingAngle);

        // Generate the group of segments for the current ring
        for (int seg = 0; seg <= segments; seg++)
        {
            float x0 = r0 * sinf(seg * deltaSegAngle);
            float z0 = r0 * cosf(seg * deltaSegAngle);

            // Add one vertex to the strip which makes up the sphere
            *vertex++ = x0;
            *vertex++ = y0;
            *vertex++ = z0;

            if (normals)
            {
                Ogre::Vector3 vNormal = Ogre::Vector3(x0, y0, z0).normalisedCopy();
                *vertex++ = vNormal.x;
                *vertex++ = vNormal.y;
                *vertex++ = vNormal.z;
            }

            if (texCoords)
            {
                *vertex++ = (float)seg / (float)segments;
                *vertex++ = (float)ring / (float)rings;
            }

            if (ring != rings)
            {
                // each vertex (except the last) has six indices pointing to it
                *indices++ = wVerticeIndex + segments + 1;
                *indices++ = wVerticeIndex;
                *indices++ = wVerticeIndex + segments;
                *indices++ = wVerticeIndex + segments + 1;
                *indices++ = wVerticeIndex + 1;
                *indices++ = wVerticeIndex;
                wVerticeIndex++;
            }
        }
    }

    // Unlock
    vBuf->unlock();
    iBuf->unlock();
}

Ogre::MeshPtr CreateCone(const string& strName, float radius, float height, int verticesInBase)
{
    Ogre::MeshPtr cone = Ogre::MeshManager::getSingleton().createManual(
        strName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Ogre::SubMesh* coneVertex = cone->createSubMesh();
    cone->sharedVertexData = new Ogre::VertexData();

    CreateCone(cone->sharedVertexData, coneVertex->indexData, radius, height, verticesInBase);

    // Set up the cone mesh
    coneVertex->useSharedVertices = true;
    cone->_setBounds(Ogre::AxisAlignedBox(Ogre::Vector3(-radius, 0, -radius),
                                          Ogre::Vector3(radius, height, radius)),
                     false);
    cone->_setBoundingSphereRadius(math::Sqrt(height * height + radius * radius));
    cone->load();
    return cone;
}

void CreateCone(Ogre::VertexData* vertexData, Ogre::IndexData* indexData, float radius,
                float height, int verticesInBase)
{
    assert(vertexData && indexData);

    // Define the vertex format
    Ogre::VertexDeclaration* vertexDecl = vertexData->vertexDeclaration;
    vertexDecl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);

    // Allocate the vertex buffer
    vertexData->vertexCount = verticesInBase + 1;
    Ogre::HardwareVertexBufferSharedPtr vBuf =
        Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
            vertexDecl->getVertexSize(0), vertexData->vertexCount,
            Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
    Ogre::VertexBufferBinding* binding = vertexData->vertexBufferBinding;
    binding->setBinding(0, vBuf);
    float* vertex = static_cast<float*>(vBuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));

    // Allocate index buffer - cone and base
    indexData->indexCount = (3 * verticesInBase) + (3 * (verticesInBase - 2));
    indexData->indexBuffer = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
        Ogre::HardwareIndexBuffer::IT_16BIT, indexData->indexCount,
        Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
    Ogre::HardwareIndexBufferSharedPtr iBuf = indexData->indexBuffer;
    unsigned short* indices =
        static_cast<unsigned short*>(iBuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));

    // Positions: cone head and base
    for (int i = 0; i < 3; i++)
    {
        *vertex++ = 0.0f;
    }

    // Base:
    float deltaBaseAngle = (2.0f * math::pi) / verticesInBase;

    for (int i = 0; i < verticesInBase; i++)
    {
        float angle = i * deltaBaseAngle;
        *vertex++ = radius * cosf(angle);
        *vertex++ = height;
        *vertex++ = radius * sinf(angle);
    }

    // Indices:
    // Cone head to vertices
    for (int i = 0; i < verticesInBase; i++)
    {
        *indices++ = 0;
        *indices++ = (i % verticesInBase) + 1;
        *indices++ = ((i + 1) % verticesInBase) + 1;
    }

    // Cone base
    for (int i = 0; i < verticesInBase - 2; i++)
    {
        *indices++ = 1;
        *indices++ = i + 3;
        *indices++ = i + 2;
    }

    // Unlock
    vBuf->unlock();
    iBuf->unlock();
}

void CreateQuad(Ogre::VertexData* vertexData)
{
    assert(vertexData);

    vertexData->vertexCount = 4;
    vertexData->vertexStart = 0;

    Ogre::VertexDeclaration* vertexDecl = vertexData->vertexDeclaration;
    Ogre::VertexBufferBinding* bind = vertexData->vertexBufferBinding;

    vertexDecl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);

    Ogre::HardwareVertexBufferSharedPtr vbuf =
        Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
            vertexDecl->getVertexSize(0), vertexData->vertexCount,
            Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

    // Bind buffer
    bind->setBinding(0, vbuf);

    // Upload data
    float data[] = {
        -1.0f, 1.0f,
        -1.0f,    // corner 1
        -1.0f, -1.0f,
        -1.0f,    // corner 2
        1.0f,  1.0f,
        -1.0f,    // corner 3
        1.0f,  -1.0f,
        -1.0f    // corner 4
    };

    vbuf->writeData(0, sizeof(data), data, true);
}

template <>
Position Lerp(const Position& a, const Position& b, float x, float dt)
{
    return a + (b - a) * (1.0f - math::Pow(1.0f - x, dt));
}

template <>
Quat Lerp(const Quat& a, const Quat& b, float x, float dt)
{
    return Quat::Slerp(a, b, 1.0f - math::Pow(1.0f - x, dt));
}

Position EstimateHitPosition(const Position& position, float speed,
                               const Position& targetPosition, const Vec3& targetVelocity)
{
    Vec3 toTarget = targetPosition.GetRelativeToPoint(position);

    // Here we're solving a quadratic
    float a = targetVelocity.Dot(targetVelocity) - speed * speed;
    float b = 2.0f * targetVelocity.Dot(toTarget);
    float c = toTarget.Dot(toTarget);

    float p = -b / (2.0f * a);
    float q = math::Sqrt((b * b) - 4.0f * a * c) / (2.0f * a);
    float t1 = p - q;
    float t2 = p + q;
    float t;

    if (t1 > t2 && t2 > 0)
        t = t2;
    else
        t = t1;

    // Now we have the time before impact - calculate the position at the time
    return targetPosition + targetVelocity * t;
}

string GenerateName(const string& prefix /*= "Unnamed"*/)
{
    static std::map<string, int> countMap;
    if (countMap.find(prefix) == countMap.end())
        countMap[prefix] = 0;
    return prefix + std::to_string(++countMap[prefix]);
}

string PadString(const string& str, uint length)
{
    string out = str;
    if (length > out.size())
        out.insert(out.size(), length - out.size(), ' ');
    return out;
}

std::vector<string>& Split(const string& s, char delim, std::vector<string>& elems)
{
    std::stringstream ss(s);
    string item;
    while (std::getline(ss, item, delim))
        elems.push_back(item);
    return elems;
}

string Concat(const vector<string>& vec, const string& delim)
{
    string out;
    for (uint i = 0; i < vec.size() - 1; ++i)
        out += vec[i];
    if (!vec.empty())
        out += vec.back();
    return out;
}

std::default_random_engine engine;

int randomInt(int min /*= 0*/, int max /*= INT_MAX*/)
{
    return std::uniform_int_distribution<int>(min, max)(engine);
}

uint randomUnsigned(uint min /*= 0*/, uint max /*= UINT_MAX*/)
{
    return std::uniform_int_distribution<uint>(min, max)(engine);
}

float randomFloat(float min /*= 0.0f*/, float max /*= 1.0f*/)
{
    return std::uniform_real_distribution<float>(min, max)(engine);
}

double randomDouble(double min /*= 0.0f*/, double max /*= 1.0f*/)
{
    return std::uniform_real_distribution<double>(min, max)(engine);
}

Vec2 randomVec2(const Vec2& min /*= Vec2(-1.0f)*/, const Vec2& max /*= Vec2(1.0f)*/)
{
    return Vec2(std::uniform_real_distribution<float>(min.x, max.x)(engine),
                std::uniform_real_distribution<float>(min.y, max.y)(engine));
}

Vec3 randomVec3(const Vec3& min /*= Vec3(-1.0f)*/, const Vec3& max /*= Vec3(1.0f)*/)
{
    return Vec3(std::uniform_real_distribution<float>(min.x, max.x)(engine),
                std::uniform_real_distribution<float>(min.y, max.y)(engine),
                std::uniform_real_distribution<float>(min.z, max.z)(engine));
}

Vec4 randomVec4(const Vec4& min /*= Vec4(-1.0f)*/, const Vec4& max /*= Vec4(1.0f)*/)
{
    return Vec4(std::uniform_real_distribution<float>(min.x, max.x)(engine),
                std::uniform_real_distribution<float>(min.y, max.y)(engine),
                std::uniform_real_distribution<float>(min.z, max.z)(engine),
                std::uniform_real_distribution<float>(min.w, max.w)(engine));
}

namespace time
{
    TimePoint Now()
    {
        static Ogre::Timer timer;
        return static_cast<TimePoint>(timer.getMicroseconds()) * 1e-6;
    }

    Duration GetElapsed(TimePoint tp)
    {
        return Now() - tp;
    }

    string Format(time_t time, const string& formatString)
    {
#if DW_PLATFORM == DW_WIN32
        std::stringstream out;
        out << std::put_time(std::gmtime(&time), formatString.c_str());
        return out.str();
#else
        return "";
#endif
    }
}

NAMESPACE_END
