/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "Noise.h"

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

float PerlinNoise::Noise(float x)
{
    return PerlinNoise1(x);
}

float PerlinNoise::Noise(float x, float y)
{
    float vec[2];
    vec[0] = x;
    vec[1] = y;
    return PerlinNoise2(vec);
}

float PerlinNoise::Noise(float x, float y, float z)
{
    float vec[3];
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
    return PerlinNoise3(vec);
}

void PerlinNoise::Init()
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

        Normalise2(g2[i]);

        for (j = 0; j < 3; j++)
        {
            g3[i][j] = (float)((rand() % (B + B)) - B) / B;
        }

        Normalise3(g3[i]);
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

float PerlinNoise::PerlinNoise1(float arg)
{
    int terms = mOctaves;
    float result = 0.0f;
    float amp = mAmplitude;

    arg *= mFrequency;

    for (int i = 0; i < terms; i++)
    {
        result += Noise1(arg) * amp;
        arg *= 2.0f;
        amp *= 0.5f;
    }

    return result;
}

float PerlinNoise::PerlinNoise2(float vec[2])
{
    int terms = mOctaves;
    float result = 0.0f;
    float amp = mAmplitude;

    vec[0] *= mFrequency;
    vec[1] *= mFrequency;

    for (int i = 0; i < terms; i++)
    {
        result += Noise2(vec) * amp;
        vec[0] *= 2.0f;
        vec[1] *= 2.0f;
        amp *= 0.5f;
    }

    return result;
}

float PerlinNoise::PerlinNoise3(float vec[3])
{
    int terms = mOctaves;
    float result = 0.0f;
    float amp = mAmplitude;

    vec[0] *= mFrequency;
    vec[1] *= mFrequency;
    vec[2] *= mFrequency;

    for (int i = 0; i < terms; i++)
    {
        result += Noise3(vec) * amp;
        vec[0] *= 2.0f;
        vec[1] *= 2.0f;
        vec[2] *= 2.0f;
        amp *= 0.5f;
    }

    return result;
}

float PerlinNoise::Noise1(float arg)
{
    int bx0, bx1;
    float rx0, rx1, sx, t, u, v, vec[1];

    vec[0] = arg;

    if (mStart)
    {
        srand(mSeed);
        mStart = false;
        Init();
    }

    setup(0, bx0, bx1, rx0, rx1);

    sx = s_curve(rx0);

    u = rx0 * g1[p[bx0]];
    v = rx1 * g1[p[bx1]];

    return math::Lerp(sx, u, v);
}

float PerlinNoise::Noise2(float vec[2])
{
    int bx0, bx1, by0, by1, b00, b10, b01, b11;
    float rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
    int i, j;

    if (mStart)
    {
        srand(mSeed);
        mStart = false;
        Init();
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

float PerlinNoise::Noise3(float vec[3])
{
    int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
    float rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
    int i, j;

    if (mStart)
    {
        srand(mSeed);
        mStart = false;
        Init();
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

void PerlinNoise::Normalise2(float v[2])
{
    float s;

    s = (float)sqrt(v[0] * v[0] + v[1] * v[1]);
    s = 1.0f / s;
    v[0] = v[0] * s;
    v[1] = v[1] * s;
}

void PerlinNoise::Normalise3(float v[3])
{
    float s;

    s = (float)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    s = 1.0f / s;

    v[0] = v[0] * s;
    v[1] = v[1] * s;
    v[2] = v[2] * s;
}

NAMESPACE_END
