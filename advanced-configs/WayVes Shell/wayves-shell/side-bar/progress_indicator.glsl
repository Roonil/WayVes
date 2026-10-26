#include ":angular/structs.glsl"
#include ":matugenColors/colors.glsl"
#include ":utils/post-processing/glow/structs.glsl"
#include ":utils/post-processing/rotate/structs.glsl"

#define coordinateRotation 0.

#define fragmentAngle .1

#define visualiserMode 0
#define visualiserDirections 0

#define isSecondary 0.

#define isPotato 0.

uniform float progress = 100;

float progressFraction = progress / 100.;
float edgeFraction = .9999;

float connectorHeight = 5;
float connectorBorderSize = 1.;

float innerSoftness = 2;
float outerSoftness = 2;

vec4 connectorColor = mix(vec4(matugenColors.primary.xyz, .8), vec4(1, 1, 1, .8), .35);
vec4 connectorBorderColor = matugenColors.outline;

void init()
{

    particle.audio.multiplier = 0;
    particle.connector.left.enable = 1;
    particle.connector.right.enable = 1;

    circle.angleOffset = 180;
    circle.restrictCircleAngle = 0;

    circle.radius = int(resolution.x / 2. - 3 * (connectorHeight + connectorBorderSize));
    circle.endAnglePadding = mix(0., 360 * (connectorHeight + 2 * (connectorBorderSize)) / (TWOPI * circle.radius),
                                 step(progressFraction, edgeFraction));
    circle.startAnglePadding =
        clamp(360. - progressFraction * (360 - circle.endAnglePadding), circle.endAnglePadding, 360.);
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

    // circle.borderSize = 2 * int(connectorBorderSize + connectorHeight);
    circle.borderSize = 1;
    circle.borderColor = vec4(0.2902, 0.2902, 0.2902, 1.0);

    particle.offset.y -= 1.5;

    circle.innerSoftness = 2;
    circle.outerSoftness = 2;

    particle.connector.jointMode = 1;

    float audioRiseMinHeight = min(.15, .3 * progressFraction);
    float audioRiseValue = 2 * (texture(audioR, .1).x);
    // audioRiseValue *= min(.75 * progressFraction, (progressFraction - audioRiseMinHeight));
    audioRiseValue *= (progressFraction - audioRiseMinHeight) * .75;

    float audioFluidAmplitude = 2.3;
    float audioFluidFrequency = 2.7;

    float audioFluidVelocity = .09;

    float audioEdge = 360. * (1. - audioRiseMinHeight - audioRiseValue) - circle.startAnglePadding;
    float audioEdgeSoftness = 360. / (TWOPI * max(particle.fragment.distanceFromCenter, 1.));

    float audioPosition = smoothstep(
        audioEdge - audioEdgeSoftness, audioEdge + audioEdgeSoftness,
        particle.fragment.currentAngle +
            audioFluidAmplitude *
                sin(audioFluidFrequency * (particle.fragment.distanceFromCenter - circle.radius - circle.borderSize) /
                        (connectorHeight + connectorBorderSize) -
                    audioFluidVelocity * time));

    vec4 audioFluidColor = vec4(mix(matugenColors.sourceColor.xyz, matugenColors.primary.xyz, .5), 1);
    audioFluidColor = mix(audioFluidColor, vec4(matugenColors.surfaceDisabled.xyz, audioFluidColor.w),
                          particle.fragment.currentAngle / (360. * progressFraction));

    particle.connector.left.color = mix(mix(connectorColor, audioFluidColor, audioPosition), connectorColor, isPotato);
    particle.connector.left.color =
        mix(particle.connector.left.color, interpolateHue(particle.connector.left.color, .8, 1, 8), isSecondary);

    particle.connector.left.borderColor = connectorBorderColor;

    particle.connector.left.height = connectorHeight;
    particle.connector.left.borderSize = connectorBorderSize;

    particle.connector.left.innerSoftness = innerSoftness;
    particle.connector.left.outerSoftness = outerSoftness;

    particle.connector.right = particle.connector.left;
}

void setParticleDownProps()
{
}

void modifySDFs()
{
    sdfs[CIRCLE_OUTER_SDF] *= 1. - sdfs[PARTICLE_UP_LEFT_CONNECTOR_INNER_SDF];
    sdfs[CIRCLE_OUTER_SDF] *= 1. - sdfs[PARTICLE_UP_LEFT_CONNECTOR_OUTER_SDF];

    sdfs[CIRCLE_OUTER_SDF] *= 1. - sdfs[PARTICLE_UP_RIGHT_CONNECTOR_INNER_SDF];
    sdfs[CIRCLE_OUTER_SDF] *= 1. - sdfs[PARTICLE_UP_RIGHT_CONNECTOR_OUTER_SDF];
}
