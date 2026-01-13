#include "angular/structs.glsl"
#include "utils/post-processing/glow/structs.glsl"

#define coordinateRotation 0.

#define audioRadialHeight 400
#define minBarHeight vec2(0)
#define fragmentAngle .01

#define weight 1.3

#define centerCoords vec2(r_resolution / 2.)

#define circleRadius 100
#define circleBorderSize 0
#define innerBorderAntiAlias 0
#define outerBorderAntiAlias 0.

#define visualiserMode 1
#define visualiserDirections 0

void init()
{
    bar.audio.multiplier = audioRadialHeight;
}

void audioFetch(inout float fetchedAudio, float n, float lastN)
{
}

void setOffsets(float direction, inout vec2 particleOffset, inout vec2 barOffset, inout vec2 barSizeOffset, vec2 barAudio, vec2 particleAudio, float xCoordinate, float n, float lastN)
{
}

void setProps()
{
    bar.size = vec3(0, .5, 0);

    bar.color = vec4(0, 0, 0, 1.0);
    bar.borderColor = vec4(0, 0, 0, 1);
    circle.color = vec4(0.0, 0.0, 0.0, .697);
    circle.borderColor = vec4(0.0, 0.0, 0.0, 1.0);

    circle.radius = circleRadius;
    circle.borderSize = circleBorderSize;

    bar.innerSoftness = vec3(6, .0, 0);
    bar.outerSoftness = vec3(6, 0, 0);
}

void setParticleDownProps()
{
}

void modifySDFs()
{
}

void setGlow0(inout Glow glow)
{
    glow.blendMode = 0;
    glow.mixAlpha = 0;

    glow.size = 12.5;

    glow.intensity = 3.5;
    glow.directions = 16.0;
    glow.quality = 6.0;
    glow.color = vec4(0, 0, 0, 1.0);
    glow.brightnessOffset = .1;
    glow.lightStrength = .5;
}
