#include "linear/structs.glsl"

#include ":utils/post-processing/glow/structs.glsl"

#define coordinateRotation 0

#define fragmentWidth 18
#define leftPadding 0
#define rightPadding 0

#define visualiserDirections 0
#define visualiserMode 1

void audioFetch(inout float fetchedAudio, float n, float lastN)
{
}

void init()
{

    audioSettings.mode = 0;

    bar.audio.multiplier = 110;

    audioSettings.combineChannels = 1;
}

void setOffsets(float direction, inout vec2 particleOffset, inout vec2 barOffset, inout vec2 barSizeOffset, vec2 barAudio, vec2 particleAudio, float xCoordinate, float n, float lastN)
{
}

void setProps()
{
    bar.type = 1;

    bar.size = vec3(7, 13, 7);
    bar.borderSize = vec3(2, 0.5, 2);

    bar.offset.y = -35;

    bar.outerSoftness = vec3(1., 1., 1);
    bar.innerSoftness = vec3(0, 1.5, 0);

    bar.color = vec4(0.1647, 0.0, 0.2039, 0.164);
    bar.borderColor = vec4(1);

    bar.upCap.enable = true;
    bar.upCap.rate = 0.2;
    bar.upCap.offset.y = 0.;
    bar.upCap.size.y = 2.;
    bar.upCap.size.x = 9.;
    bar.upCap.color = vec4(1.0);
}

void setParticleDownProps()
{
}

void modifySDFs()
{
}

void setGlow0(inout Glow glow)
{
    glow.size = 3.5;
    glow.directions = 16;
    glow.color = vec4(0.9529, 0.2431, 0.2157, 1.0);
    glow.brightnessOffset = .4;
    glow.quality = 3;
    glow.blendMode = 0;
    glow.mixAlpha = 0;
    glow.lightStrength = 1;
}
