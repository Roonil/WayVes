#include "angular/structs.glsl"
#include "angular/zOrders.glsl"
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

    bar.color = vec4(0.3098, 0.0667, 0.7608, 0.705);
    bar.borderColor = vec4(0, 0, 0, 0);
    circle.color = vec4(0.0, 0.0, 0.0, 0.0);
    circle.borderColor = vec4(0.0, 0.0, 0.0, 0.0);

    circle.radius = circleRadius;
    circle.borderSize = circleBorderSize;

    bar.bgColor = vec4(1);

    bar.innerSoftness = vec3(2, 0, 0);
    bar.outerSoftness = vec3(2, 0, 0);
}

void setParticleDownProps()
{
}

void modifySDFs()
{
    float w = abs(bar.fragment.span);
    sdfs[BAR_BG_SDF] = 1. - smoothstep((bar.audio.current.y) - 150.4, (bar.audio.current.y), bar.fragment.distanceFromCenter - circle.radius - 50);

    sdfs[BAR_BG_SDF] *= 3. * bar.audio.current.y / bar.audio.multiplier;

    setBarGroupCBM(0, 0, 0, 0, 0);
    setBarGroupPassThrough(1, 1, 1);

    applyZOrders();

    sdfs[BAR_BG_SDF] *= (1. - sdfs[BAR_OUTER_SDF]) * (1. - sdfs[BAR_INNER_SDF]) * (1 - sdfs[CIRCLE_INNER_SDF]) * (1 - sdfs[CIRCLE_OUTER_SDF]);
}

void setGlow0(inout Glow glow)
{
    glow.blendMode = 0;
    glow.mixAlpha = 0;

    glow.size = 23.5;

    glow.intensity = 4.5;
    glow.directions = 16.0;
    glow.quality = 9.0;
    glow.color = vec4(0.0588, 0.1686, 0.4745, 1.0);
    glow.brightnessOffset = .1;
    glow.lightStrength = .5;
}
