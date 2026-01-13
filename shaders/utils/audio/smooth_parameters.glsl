// Portions adapted from GLava's Audio Smoothing Shader by jarcode-foss
// Licensed under GPL-3.0

#include "utils.glsl"

#define ROUND_FORMULA sinusoidal

uniform int sample_mode;
uniform float sample_hybrid_weight;
uniform float sample_scale;
uniform float sample_range;
uniform float smooth_factor;

float scale_audio(float idx)
{
    return -log((-(sample_range)*idx) + 1) / (sample_scale);
}

float iscale_audio(float idx)
{
    return -log((sample_range)*idx) / (sample_scale);
}

float smooth_audio(in sampler1D tex, int tex_sz, highp float idx)
{

    float smin = scale_audio(clamp(idx - smooth_factor, 0, 1)) * tex_sz,
          smax = scale_audio(clamp(idx + smooth_factor, 0, 1)) * tex_sz;
    float m = ((smax - smin) / 2.0F), s, w;
    float rm = smin + m;

    if (sample_mode == 0) {
        float avg = 0, weight = 0;
        for (s = smin; s <= smax; s += 1.0F) {
            w = ROUND_FORMULA(clamp((m - abs(rm - s)) / m, 0, 1));
            weight += w;
            avg += texelFetch(tex, int(round(s)), 0).r * w;
        }
        avg /= weight;
        return avg;
    } else if (sample_mode == 2) {
        float vmax = 0, avg = 0, weight = 0, v;
        for (s = smin; s < smax; s += 1.0F) {
            w = ROUND_FORMULA(clamp((m - abs(rm - s)) / m, 0, 1));
            weight += w;
            v = texelFetch(tex, int(round(s)), 0).r * w;
            avg += v;
            if (vmax < v)
                vmax = v;
        }
        return (vmax * (1 - sample_hybrid_weight)) + ((avg / weight) * sample_hybrid_weight);
    } else if (sample_mode == 1) {
        float vmax = 0, v;
        for (s = smin; s < smax; s += 1.0F) {
            w = texelFetch(tex, int(round(s)), 0).r * ROUND_FORMULA(clamp((m - abs(rm - s)) / m, 0, 1));
            if (vmax < w)
                vmax = w;
        }
        return vmax;
    }
}

float smooth_audio_adj(in sampler1D tex, int tex_sz, highp float idx,
    highp float pixel)
{
    float al = smooth_audio(tex, tex_sz, max(idx - pixel, 0.0F)),
          am = smooth_audio(tex, tex_sz, idx),
          ar = smooth_audio(tex, tex_sz, min(idx + pixel, 1.0F));
    return (al + am + ar) / 3.0F;
}

#ifdef TWOPI
#undef TWOPI
#endif
#ifdef PI
#undef PI
#endif
