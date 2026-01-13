#include "linear/structs.glsl"

#define coordinateRotation 0
#define fragmentWidth 14

#define leftPadding 0
#define rightPadding 0

#define visualiserDirections 0
#define visualiserMode 1

void init()
{
    bar.audio.multiplier = 70;
}

void audioFetch(inout float fetchedAudio, float n, float lastN)
{
}

void setOffsets(float direction, inout vec2 particleOffset, inout vec2 barOffset, inout vec2 barSizeOffset, vec2 barAudio, vec2 particleAudio, float xCoordinate, float n, float lastN)
{
}

void setProps()
{
    bar.color = vec4(1.0, 0.0, 0.0, 1.0);
    bar.size = vec3(3, 5, 2);
    bar.borderSize = vec3(2);

    bar.borderColor = vec4(1.0, 1.0, 1.0, 1.0);
    bar.innerSoftness = vec3(1);
    bar.outerSoftness = vec3(1, 1, 1);
}

void setParticleDownProps()
{
}

void modifySDFs()
{
}
