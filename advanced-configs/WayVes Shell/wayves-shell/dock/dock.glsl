#include ":angular/structs.glsl"
#include ":matugenColors/colors.glsl"
#include ":utils/post-processing/glow/structs.glsl"

#define coordinateRotation 0.

uniform float sweepAngle = 120;
uniform float numElements = 9;

uniform int iconHoveredIndex = -1;
uniform int iconClickedIndex = -1;

float gapAngle = 1.;

vec4 barColor = matugenColors.isDarkMode ? matugenColors.primaryContainer : matugenColors.primaryContainer;
vec4 borderColor = matugenColors.outline;
vec4 particleColor = matugenColors.onPrimaryContainer;

#define fragmentAngle (sweepAngle / (numElements - 1))

#define visualiserMode 2
#define visualiserDirections 2

#define connectorColor vec4(1)

#define isPotato 0.

void setCorrectAlpha()
{
    barColor.w *= .7;
    particleColor.w *= .7;
}

void init()
{
    setCorrectAlpha();

    bar.audio.multiplier = sign(abs(1. - isPotato)) * 2.5;
    particle.audio.multiplier = sign(abs(1. - isPotato)) * 400;
    particle.connector.left.enable = 1;
    particle.connector.right.enable = 1;
    circle.maxAngle = sweepAngle + fragmentAngle;
    circle.angleOffset = -circle.maxAngle / 2.;
    circle.radius = int(r_resolution.y);
    circle.center.y -= circle.radius / 2;

    // circle.restrictCircleAngle = 1;
    // bar.mergeLeftBar = 1;
    // bar.mergeRightBar = 1;
    // bar.mergeEnds = 1;
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
    // bar.type = 1;
    bar.color = barColor;
    // bar.color.xyz = mix(barColor.xyz, matugenColors.primaryContainer.xyz, .5);
    bar.borderSize = vec3(15, .25, 15);

    bar.size = vec3(0, fragmentAngle - gapAngle, 300);
    bar.innerSoftness = vec3(10, .3, 10);
    bar.outerSoftness = vec3(20, .3, 20);

    bar.innerSoftness.y += mix(0., 1., 2. * abs(bar.fragment.currentAngle - sweepAngle / 2.) / sweepAngle);
    bar.outerSoftness.y += mix(0., .6, 2. * abs(bar.fragment.currentAngle - sweepAngle / 2.) / sweepAngle);

    bar.color = interpolateHue(bar.color, .07 + (bar.audio.current.x + bar.audio.current.y) / 2.,
                               2 * abs(bar.fragment.n - bar.fragment.lastN / 2.), bar.fragment.lastN);

    bar.color = mix(bar.color, matugenColors.sourceColor, (bar.audio.current.x + bar.audio.current.y) / 2.);
    bar.color = mix(bar.color, matugenColors.isDarkMode ? vec4(1) : vec4(0),
                    .8 * (bar.audio.current.x + bar.audio.current.y) / 2.);

    bar.color.w = mix(bar.color.w, .7, 2. * abs(bar.fragment.n - bar.fragment.lastN / 2.) / bar.fragment.lastN);

    bar.borderColor = mix(mix(borderColor, bar.color, .5), vec4(1.0), .5);
    bar.borderColor = mix(bar.borderColor, vec4(1), (bar.audio.current.x + bar.audio.current.y) / 2.);

    circle.borderSize = 0;
    circle.innerSoftness = 1;
    circle.outerSoftness = 1;

    bar.audio.current = vec2(0);

    circle.borderColor = vec4(1.0, 1.0, 1.0, 1.0);

    if (bar.fragment.n == bar.fragment.lastN - iconHoveredIndex && isPotato == 0.)
    {
        bar.color = matugenColors.isDarkMode ? matugenColors.primary : matugenColors.surfaceContainer;
    }

    if (bar.fragment.n == bar.fragment.lastN - iconClickedIndex && isPotato == 0.)
    {
        bar.color = matugenColors.secondary;
    }
}

void setParticleDownProps()
{
    particle.color = particleColor;
    particle.radius = 60;
    particle.outerSoftness = 80 + (particle.audio.current.x + particle.audio.current.y) / 2.;
    particle.innerSoftness = 80 + (particle.audio.current.x + particle.audio.current.y) / 2.;
    particle.offset.y = -150;
    particle.audio.current = vec2(0);
}

void modifySDFs()
{
}

void setGlow0(inout Glow glow)
{
    glow.size = vec2(40, 80);
    glow.intensity = 0.5;
    glow.color = vec4(matugenColors.primary.xyz, 1);
    glow.mixAlpha = 1;
    glow.quality = 12;
    glow.directions = 16;
    vec2 vecFromCenter = glow.coords - r_resolution / 2;
    float currentAngle = atan(vecFromCenter.y, vecFromCenter.x) / PI;
    currentAngle += 1.;
    currentAngle *= 180.;
    glow.onTop = 0;
    glow.offsetAngle = mix(60, 120, 1. - 2 * (abs(glow.coords.x - resolution.x / 2.) / resolution.x));
}

void setGlow1(inout Glow glow)
{
    glow.size = vec2(22, 80);
    glow.intensity = 3.5;
    glow.color = vec4(0.0, 0.0, 0.0, .5);
    glow.mixAlpha = 1;
    glow.quality = 12;
    glow.directions = 1;
    glow.offsetAngle = 0;
    glow.onTop = 0;

    vec2 vecFromCenter = glow.coords - r_resolution / 2;
    float currentAngle = atan(vecFromCenter.y, vecFromCenter.x) / PI;
    currentAngle += 1.;
    currentAngle *= 180.;

    glow.offsetAngle = mix(60, 120, 1. - 2 * (abs(glow.coords.x - resolution.x / 2.) / resolution.x));
}