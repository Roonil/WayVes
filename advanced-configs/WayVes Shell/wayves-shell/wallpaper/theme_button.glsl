#include ":angular/structs.glsl"
#include ":matugenColors/colors.glsl"
#include ":utils/post-processing/glow/structs.glsl"
#include ":utils/post-processing/rotate/structs.glsl"

#define coordinateRotation 0.

#define fragmentAngle 22.5

#define visualiserMode 0
#define visualiserDirections 0

void init()
{
    particle.audio.multiplier = 70;

    circle.angleOffset = -(fragmentAngle) / 2. + (matugenColors.isDarkMode ? 1 : -1) * time * .5;
}

void audioFetch(inout float fetchedAudio, float n, float lastN)
{
#define W2 5.3
#define EW exp(-W2)

    float d = 1. - fetchedAudio;

    fetchedAudio = ((exp(-d * d * W2) - EW));
    fetchedAudio *= 1. - step(abs(fetchedAudio), .005);

    fetchedAudio *= (mod(int(n), 2) * 2 - 1);
}

void setOffsets(float direction, inout vec2 particleOffset, inout vec2 barOffset, inout vec2 barSizeOffset,
                vec2 barAudio, vec2 particleAudio, float xCoordinate, float n, float lastN)
{
}

float getMixVal(float startValue, float endValue)
{
    return smoothstep(startValue * resolution.y, endValue * resolution.y, bar.fragment.coords.y);
}

vec4 grade3(vec4 core, vec4 mid, vec4 rim, float t)
{
    return t < .5 ? mix(core, mid, t * 2.) : mix(mid, rim, t * 2. - 1.);
}

void setCircleProps()
{

    vec4 color1 = matugenColors.isDarkMode ? vec4(0.8549, 0.6039, 1.0, 1.0) : vec4(0.9294, 0.2039, 0.4471, 1);
    vec4 color2 = matugenColors.isDarkMode ? vec4(0.1765, 0.4824, 0.6588, 1.0) : vec4(1.0, 0.902, 0.0, 1.0);

    vec4 sunCore = vec4(1.0, 0.9686, 0.8235, 1.0);
    vec4 sunMid = vec4(1.0, 0.7843, 0.2157, 1.0);
    vec4 sunRim = vec4(0.9725, 0.4235, 0.1255, 1.0);

    vec4 moonCore = vec4(0.9412, 0.9529, 1.0, 1.0);
    vec4 moonMid = vec4(0.9608, 0.7529, 0.8706, 1.0);
    vec4 moonRim = vec4(0.8667, 0.6549, 1.0, 1.0);

    // Looks pretty nice
    // vec4 moonCore = vec4(0.9412, 0.9529, 1.0, 1.0);
    // vec4 moonMid = vec4(0.9608, 0.7529, 0.8706, 1.0);
    // vec4 moonRim = vec4(0.8667, 0.6549, 1.0, 1.0);

    float t = getMixVal(.2 + clamp(texture(audioR, .1).x * .5, 0., .3), .7 + clamp(texture(audioR, .4).x * .5, 0., .3));

    circle.color =
        matugenColors.isDarkMode ? grade3(moonCore, moonMid, moonRim, t) : grade3(sunCore, sunMid, sunRim, t);

    circle.radius = 60;
    circle.radius = 20;

    circle.innerSoftness = 1;
}

void setProps()
{
    setCircleProps();

    // particle.color = matugenColors.primary;
    // particle.borderColor = matugenColors.onPrimary;

    particle.radius = 4;
    particle.radius = 2;
    circle.borderSize = 13;
    particle.color = matugenColors.surface;

    particle.borderSize = 1;
    particle.borderColor = matugenColors.outline;

    particle.innerSoftness = 1 + length(particle.audio.current);
    particle.outerSoftness = 1;

    particle.audio.current = sign(particle.audio.current) * min(abs(particle.audio.current), vec2(4));
}

void setParticleDownProps()
{
}

void modifySDFs()
{
}

void setGlow0(inout Glow glow)
{
    glow.blendMode = 1;
    glow.mixAlpha = 1;

    glow.colorInfluence = 1;

    glow.size = vec2(12);
    glow.intensity = 0.5 + texture(audioR, .1).x * 6;
    glow.directions = 16.0;
    glow.quality = 6.0;

    glow.color = matugenColors.isDarkMode ? vec4(0.251, 0.0, 1.0, 0.438) : vec4(1.0, 0.0, 0.0, 0.399);
    glow.brightnessOffset = .0;
    glow.lightStrength = .5;
}
