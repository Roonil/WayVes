#include "angular/structs.glsl"
#include "utils/colors.glsl"

#include "utils/post-processing/glow/structs.glsl"

#define coordinateRotation 0.

#define isMinute false
#define isHour false

#define fragmentAngle 15.

#define visualiserMode 1
#define visualiserDirections 0

uniform float trackPosition = 0;

void init()
{
    bar.audio.multiplier = 120;
    circle.radius = 130;
    circle.angleOffset = -7.5;
}

void audioFetch(inout float fetchedAudio, float n, float lastN)
{
#define W2 5.3
#define EW exp(-W2)

    float d = 1. - fetchedAudio;

    fetchedAudio = ((exp(-d * d * W2) - EW));
    fetchedAudio *= 1. - step(abs(fetchedAudio), .005);
}

void setOffsets(float direction, inout vec2 particleOffset, inout vec2 barOffset, inout vec2 barSizeOffset, vec2 barAudio, vec2 particleAudio, float xCoordinate, float n, float lastN)
{
}

void setProps()
{
    circle.color = vec4(0.0, 0.2314, 0.6275, 0.308);
    circle.innerSoftness = 9;
    bar.color = mix(vec4(1), vec4(0.7608, 0.7608, 0.7608, 0.5), sign(mod(bar.fragment.n, 2)));
    bar.borderColor = vec4(0.0, 0.0, 0.0, 1.0);
    bar.borderSize = vec3(0, 1.3, 0);

    circle.borderColor = vec4(0.0, 0.0431, 0.1216, 1.0);

    circle.borderSize = 2;
    circle.outerSoftness = 2.;
    bar.color = mix(bar.color, interpolateHue(vec4(0.0, 0.8, 1.0, 1.0), (sin(time * 0.1) + 1.) / 2., trackPosition * bar.fragment.n, bar.fragment.lastN), 1. - step(trackPosition, .5 * bar.fragment.currentAngle / PI));

    bar.size = vec3(0, 6, 0);

    bar.type = 2;
    bar.offset.y = -15;
    bar.innerSoftness = vec3(0, 1, 3);
    bar.outerSoftness = vec3(0, 2.5, 1);
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

    glow.size = 9;
    glow.intensity = 3.5;
    glow.directions = 16.0;
    glow.quality = 6.0;
    glow.color = vec4(0.0, 0.0157, 0.0431, 1.0);
    glow.brightnessOffset = .2;
    glow.lightStrength = .7;
}
