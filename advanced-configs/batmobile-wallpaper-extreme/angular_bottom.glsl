#include ":utils/post-processing/glow/structs.glsl"
#include "angular/structs.glsl"
#include "angular/zOrders.glsl"

#define coordinateRotation 0.

#define audioRadialHeight 800

#define fragmentAngle 0.16

#define weight 3.3

#define circleRadius 5000
#define circleBorderSize 0
#define innerBorderAntiAlias 0.
#define outerBorderAntiAlias 0.

#define visualiserMode 1
#define visualiserDirections 0

void init()
{
    bar.audio.multiplier = audioRadialHeight;
    bar.mergeLeftBar = 1;
    bar.mergeRightBar = 1;
    circle.center = vec2(r_resolution.x / 2., r_resolution.y / 2. - 5050);
    circle.angleOffset = -12.;
    circle.maxAngle = 24.1;
    circle.restrictCircleAngle = 1;
}

void audioFetch(inout float fetchedAudio, float n, float lastN)
{
}

void setOffsets(float direction, inout vec2 particleOffset, inout vec2 barOffset, inout vec2 barSizeOffset,
                vec2 barAudio, vec2 particleAudio, float xCoordinate, float n, float lastN)
{
}

void setProps()
{
    bar.size = vec3(4, .13, 0);
    bar.borderSize = vec3(1.3, .01, 0);
    bar.color = vec4(0.0, 0.0, 0.0, 0.685);
    bar.borderColor = vec4(1.0, 0.549, 0.8275, 1.0);
    circle.color = bar.color;
    circle.borderColor = vec4(.0);

    circle.radius = circleRadius;
    circle.borderSize = circleBorderSize;

    bar.innerSoftness = vec3(2.8, .02, 0);
    bar.outerSoftness = vec3(3.1, .03, 0);

    bar.bgColor = vec4(0.3882, 0.0196, 0.6667, 0.775);
}

void setParticleDownProps()
{
}

void modifySDFs()
{

    float w = abs(bar.fragment.span);
    sdfs[BAR_BG_SDF] = 1. - smoothstep((bar.audio.current.y) - 150.4, (bar.audio.current.y),
                                       bar.fragment.distanceFromCenter - circle.radius - 50);

    sdfs[BAR_BG_SDF] *= 3. * bar.audio.current.y / bar.audio.multiplier;

    setBarGroupCBM(0, 0, 0, 0, 0);
    setBarGroupPassThrough(1, 1, 1);

    applyZOrders();

    sdfs[BAR_BG_SDF] *= (1. - sdfs[BAR_OUTER_SDF]) * (1. - sdfs[BAR_INNER_SDF]) * (1 - sdfs[CIRCLE_INNER_SDF]) *
                        (1 - sdfs[CIRCLE_OUTER_SDF]);
}

void setGlow0(inout Glow glow)
{
    glow.blendMode = 1;
    glow.mixAlpha = 0;

    glow.size = vec2(90);
    glow.offsetAngle = 270;
    glow.intensity = 1.5;
    glow.directions = 4.0;
    glow.quality = 8.0;
    glow.color = vec4(0.4667, 0.0314, 0.2627, 1.0);
    glow.brightnessOffset = .2;
    glow.lightStrength = .7;
}
