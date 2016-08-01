/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

namespace dw {

#define SAMPLE_SIZE 1024
class DW_API PerlinNoise {
public:
    PerlinNoise(int octaves, float freq, float amp, int seed);

    // Generate noise
    float noise(float x);
    float noise(float x, float y);
    float noise(float x, float y, float z);

private:
    float perlin1(float arg);
    float perlin2(float vec[2]);
    float perlin3(float vec[3]);

    void init();

    // Helper functions
    float noise1(float arg);
    float noise2(float vec[2]);
    float noise3(float vec[3]);
    void normalise2(float v[2]);
    void normalise3(float v[3]);

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
}
