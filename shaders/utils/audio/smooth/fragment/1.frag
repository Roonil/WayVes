// Adapted from GLava's Audio Smoothing Shader by jarcode-foss
// Licensed under GPL-3.0

#version 330 core

#include ":utils/audio/smooth_parameters.glsl"

uniform sampler1D audioR;
out vec4 FragColor;

uniform int audioRSize;
uniform int adjacentSampleNums;

void main()
{
    float u = gl_FragCoord.x / audioRSize;
    FragColor.r = 0;
    float aRI = 1. / audioRSize;
#define adjacent(I) FragColor.r += (1. - step(float(I), 1.)) * (smooth_audio(audioR, audioRSize, u + (I - 1) * aRI) + smooth_audio(audioR, audioRSize, u - (I - 1) * aRI));

#expand adjacent adjacentSampleNums

    FragColor.r += (smooth_audio(audioR, audioRSize, u));

    FragColor.r /= 2. * (adjacentSampleNums - 1.) + 1.;
}