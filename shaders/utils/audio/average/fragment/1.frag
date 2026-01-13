// Adapted from GLava's Averaging Shader by jarcode-foss
// Licensed under GPL-3.0

#version 330 core
precision highp float;

#include ":utils/audio/utils.glsl"

uniform int avgFrames;
int _AVG_WINDOW = 1;

#define WIN_FUNC window_frame

#define SAMPLER(I) uniform sampler1D audioR##I;
#expand SAMPLER avgFrames

out vec4 FragColor;

void main()
{
    float r = 0;

    /* Disable windowing for two frames (distorts results) */
    if (avgFrames == 2)
        _AVG_WINDOW = 0

/* Use 'shallow' windowing for 3 frames to ensure the first & last
frames have a reasonable amount of weight */
#if avgFrames == 3
#define WIN_FUNC window_shallow
#endif
            ;

#define F(I) \
    r += window(I, avgFrames - 1) * texelFetch(audioR##I, int(gl_FragCoord.x), 0).r

#expand F avgFrames

    FragColor.r = (r / avgFrames);
}
