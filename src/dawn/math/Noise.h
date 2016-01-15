/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

NAMESPACE_BEGIN

#define SAMPLE_SIZE 1024
class DW_API PerlinNoise
{
public:
    PerlinNoise(int octaves, float freq, float amp, int seed);

    // Generate noise
    float Noise(float x);
    float Noise(float x, float y);
    float Noise(float x, float y, float z);

private:
    float PerlinNoise1(float arg);
    float PerlinNoise2(float vec[2]);
    float PerlinNoise3(float vec[3]);

    void Init();

    // Helper functions
    float Noise1(float arg);
    float Noise2(float vec[2]);
    float Noise3(float vec[3]);
    void Normalise2(float v[2]);
    void Normalise3(float v[3]);

    int mOctaves;
    float mFrequency;
    float mAmplitude;
    int mSeed;

    int p[SAMPLE_SIZE + SAMPLE_SIZE + 2];
    float g3[SAMPLE_SIZE + SAMPLE_SIZE + 2][3];
    float g2[SAMPLE_SIZE + SAMPLE_SIZE + 2][2];
    float g1[SAMPLE_SIZE + SAMPLE_SIZE + 2];
    bool mStart;
};

NAMESPACE_END
