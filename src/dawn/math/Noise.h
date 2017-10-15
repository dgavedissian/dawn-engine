/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

namespace dw {
// THIS IS A DIRECT TRANSLATION TO C++11 FROM THE REFERENCE
// JAVA IMPLEMENTATION OF THE IMPROVED PERLIN FUNCTION (see http://mrl.nyu.edu/~perlin/noise/)
// THE ORIGINAL JAVA IMPLEMENTATION IS COPYRIGHT 2002 KEN PERLIN
class PerlinNoise {
public:
    // Initialize with the reference values for the permutation vector.
    PerlinNoise();

    // Generate a new permutation vector based on the value of seed.
    explicit PerlinNoise(uint seed);

    double noise(double x, double y, double z);

private:
    double fade(double t);
    double lerp(double t, double a, double b);
    double grad(int hash, double x, double y, double z);

    // Permutation vector.
    std::vector<int> p;
};

/*
 * Octaves are how many layers you are putting together. If you start with big features, the number
 * of octaves determines how detailed the map will look.
 *
 * The frequency of a layer is how many points fit into the space you've created. So for the
 * mountain scale, you only need a few points, but at the rock scale you may need hundreds of
 * points. In the code above, I start with a small frequency (which equates to large features) and
 * move to higher frequencies which produce smaller details.
 *
 * The amplitude is how tall the features should be. Frequency determines the width of features,
 * amplitude determines the height. Each octave the amplitude shrinks, meaning small features are
 * also short. This doesn't have to be the case, but for this case it makes pleasing maps.
 *
 * Lacunarity is what makes the frequency grow. Each octave the frequency is multiplied by the
 * lacunarity. I use a lacunarity of 2.0, however values of 1.8715 or 2.1042 can help to reduce
 * artifacts in some algorithms. A lacunarity of 2.0 means that the frequency doubles each octave,
 * so if the first octave had 3 points the second would have 6, then 12, then 24, etc. This is used
 * almost exclusively, partly because octaves in music double in frequency. Other values are
 * perfectly acceptable, but the results will vary.
 *
 * Persistence, also called gain, is what makes the amplitude shrink (or not shrink). Each octave
 * the amplitude is multiplied by the gain. I use a gain of 0.65. If it is higher then the amplitude
 * will barely shrink, and maps get crazy. Too low and the details become miniscule, and the map
 * looks washed out. However, most use 1/lacunarity. Since the standard for lacunarity is 2.0, the
 * standard for the gain is 0.5. Noise that has a gain of 0.5 and a lacunarity of 2.0 is referred to
 * as 1/f noise, and is the industry standard.
 */
class fBmNoise {
public:
    fBmNoise(uint octaves, float frequency, float amplitude, float lacunarity = 2.0f,
             float persistence = 0.5f);
    fBmNoise(uint seed, uint octaves, float frequency, float amplitude, float lacunarity = 2.0f,
             float persistence = 0.5f);

    double noise(double x, double y, double z);

private:
    uint octaves_;
    float frequency_;
    float amplitude_;
    float lacunarity_;
    float persistence_;
    PerlinNoise noise_function_;
};
}  // namespace dw
