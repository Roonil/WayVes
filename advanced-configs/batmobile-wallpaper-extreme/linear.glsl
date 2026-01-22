#include "linear/structs.glsl"
#include "utils/post-processing/glow/structs.glsl"
#include "utils/post-processing/rotate/structs.glsl"

#define coordinateRotation 0

#define particleRadius 1
#define fragmentWidth 4
#define leftPadding 0
#define rightPadding 120
#define particleBorderSize 1
#define amplify 102.
#define weight 3.4

#define barSide 1.

#define innerParticleSoftness 2.
#define outerParticleSoftness 1.
#define particleBorderColor vec4(1.0, 0.651, 0.8431, 1.0)
#define particleColor vec4(1.0, 0.0118, 0.3725, 1.0)

#define enableDecay true

#define enableConnectors false

#define alternateAmplitude false
#define combineAudioChannels false

#define visualiserDirections 2
#define visualiserMode 0

#define audioExponentiate true

void init()
{
    particle.connector.left.enable = enableConnectors ? 1 : 0;
    particle.connector.right.enable = enableConnectors ? 1 : 0;

    audioSettings.mode = 1;
    particle.audio.multiplier = amplify;
    bar.audio.multiplier = amplify;
    int barGroupCount = int(mix(mix(mix(mix(2, 4, step(bar.fragment.n, 35)), 7, step(bar.fragment.n, 24)), 8, step(bar.fragment.n, 18)), 3, step(bar.fragment.n, 4.)));
    bar.mergeLeftBar = 1 - int(step(mod(bar.fragment.n, barGroupCount), 0));
    bar.mergeRightBar = 1 - int(step(barGroupCount - 1, mod(bar.fragment.n, barGroupCount)));

    audioSettings.combineChannels = 0;
}

void audioFetch(inout float fetchedAudio, float n, float lastN)
{
#define W2 weight* weight
#define EW exp(-W2)

    float d = 1. - fetchedAudio;

    fetchedAudio = (alternateAmplitude ? sign(mod(int(n), 2) * 2 - 1) : 1) * (!audioExponentiate ? fetchedAudio : (exp(-d * d * W2) - EW) * (W2));
    fetchedAudio *= 1. - step(abs(fetchedAudio), .005);
}

void setOffsets(float direction, inout vec2 particleOffset, inout vec2 barOffset, inout vec2 barSizeOffset, vec2 barAudio, vec2 particleAudio, float xCoordinate, float n, float lastN)
{
}

void setProps()
{
    particle.radius = particleRadius;
    particle.borderSize = particleBorderSize;
    particle.color = mix(particleColor, vec4(0.5373, 0.0078, 0.0078, 1.0), 2 * texture(audioL, .1).x);
    particle.borderColor = mix(particleBorderColor, vec4(1.0, 0.5529, 0.5529, 1.0), 2 * texture(audioL, .1).x);
    particle.innerSoftness = innerParticleSoftness;
    particle.outerSoftness = outerParticleSoftness;

    particle.reverseBottomOffset = 0;

    particle.connector.jointMode = 1;

    particle.connector.left.height = 2.1;
    particle.connector.left.borderColor = vec4(0.349, 0., 1.0, 1.0);
    particle.connector.left.borderSize = 0.75;
    particle.connector.left.color = vec4(0.9882, 0.8471, 1.0, 1.0);
    particle.connector.left.innerSoftness = 3.3;
    particle.connector.left.outerSoftness = 5.5;

    particle.connector.right = particle.connector.left;

    particle.cap.enable = enableDecay;
    particle.cap.rate = 0.4;
    particle.cap.launchFlingMultiplier = 1.4;
    particle.cap.launchVelocity = .2;
    particle.cap.elasticity = .95;
    particle.cap.elasticityMinThreshold = .8;

    particle.cap.acceleration = 1.9;
    particle.cap.dragFactor = .12;

    particle.cap.size = vec2(2, 1);

    particle.cap.offset.y = -2.;
    particle.cap.color = vec4(1.0, 0.3961, 0.6196, 1.0);

    bar.size = mix(vec3(5., 9., 5), vec3(0., 12., 0), barSide);
    bar.borderSize = mix(vec3(1, 1, 1), vec3(0, 1, 1), barSide);

    bar.outerSoftness = mix(vec3(3.3, .1, 3), vec3(0, .3, .3), barSide);
    bar.innerSoftness = mix(vec3(2.5, .3, 2.5), vec3(0, 1.3, 1.3), barSide);

    bar.offset.y = 95;

    bar.color = mix(mix(vec4(0.0392, 0.0118, 0.0784, 0.301), vec4(0.6627, 0.0706, 0.1686, 0.351), clamp(2. * abs(.5 - bar.fragment.coords.x / resolution.x) / 3., 0, 1)), vec4(0.0, 0.0, 0.0, 0.738), barSide);
    bar.borderColor = mix(vec4(1.0, 0.6706, 0.8353, 1.0), vec4(1), barSide);

    bar.downCap.enable = true;
    bar.downCap.rate = 0.01;
    bar.downCap.elasticity = .6;
    bar.downCap.launchVelocity = 0;
    bar.downCap.acceleration = .3;
    bar.downCap.launchFlingMultiplier = 2.4;
    bar.downCap.size.x = 8.;
    bar.downCap.size.y = mix(1., 2., barSide);
    bar.downCap.softness = mix(vec3(0), vec3(.5), barSide);
    bar.downCap.offset = vec2(0);
    bar.downCap.color = mix(vec4(0.6471, 0.1176, 0.9294, 0.775), vec4(0.1176, 0.7608, 0.9569, 0.863), clamp(.8 * abs(bar.audio.current.x / bar.fragment.coords.y), 0, 1));
}

void setParticleDownProps()
{
}

void modifySDFs()
{
}

#define glowSize0 5.50
#define glowIntensity0 .8
#define glowColor0 mix(vec4(0.6196, 0.0, 0.2078, 1), vec4(1.0, 0.5451, 0.5451, 1), 2 * texture(audioR, .1).x)

void setGlow0(inout Glow glow)
{
    glow.intensity = glowIntensity0;
    glow.color = glowColor0;
    glow.directions = 16;
    glow.blendMode = 0;
    glow.mixAlpha = 0;
    glow.size = glowSize0;
    glow.quality = 3;
    glow.brightnessOffset = .4;
    glow.lightStrength = 1;
}

void setRotate0(inout Rotate rotate)
{

#define tstWt 1.3
#define tW2 tstWt* tstWt
#define tEW exp(-tW2)
#define dT (1. - texture(audioR, .06).x)

    rotate.coords.x += int(-120 * abs(sin(1.5 * (clamp(((exp(-dT * dT * tW2) - tEW) * (tW2)) - .01, 0.1, .8)))));
}