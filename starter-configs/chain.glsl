#include "chain/structs.glsl"

#include "utils/colors.glsl"

#define coordinateRotation 0.

void init()
{
    chain.density = 256;
    audio.multiplier = 1.2;
}

void audioFetch(inout float fetchedAudio, float n, float lastN)
{
}

void setProps()
{
    chain.color = interpolateHue(vec4(0, 0.4, 1, 0.03), .05, 15 * (resolution.x - chain.index) * audio.value.x, resolution.x);
}