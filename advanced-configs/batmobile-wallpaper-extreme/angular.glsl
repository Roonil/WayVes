#include "angular/structs.glsl"
#include "utils/post-processing/glow/structs.glsl"
#include "utils/post-processing/rotate/structs.glsl"

#define coordinateRotation 0.

#define fragmentAngle 8.

#define visualiserMode 1
#define visualiserDirections 2

void init()
{
    audioSettings.mode = 1;
    audioSettings.reverseLeft = 1;
    audioSettings.reverseRight = 1;
    bar.audio.multiplier = 70;
    particle.audio.multiplier = 20;
    particle.connector.left.enable = 1;
    particle.connector.right.enable = 1;

    circle.maxAngle = 330;
    circle.restrictCircleAngle = 0;
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
    bar.color = vec4(0.1725, 0.9529, 0.6941, 1.0);
    bar.size = vec3(1.5);
    bar.borderSize = vec3(.5);

    bar.borderColor = vec4(0.0275, 0.9765, 0.851, 0.519);
    circle.color = vec4(0.1843, 0.0, 0.0627, 0.021) * vec4(1, 1, 1, 1. + 4.5 * texture(audioR, .1).x);
    circle.borderColor = vec4(1.0);

    particle.color = bar.color;
    particle.borderColor = bar.borderColor;

    circle.radius = 130;

    particle.connector.loop = 1;
    particle.connector.left.height = 2;
    particle.connector.left.borderSize = 1;
    particle.connector.left.color = vec4(1);
    particle.connector.left.borderColor = vec4(0.2, 0.0, 1.0, 1.0);
    particle.connector.jointMode = 1;
    particle.connector.left.innerSoftness = 4;
    particle.connector.left.outerSoftness = 4;

    particle.connector.right = particle.connector.left;

    particle.radius = 4;
    particle.color = vec4(1);

    bar.upCap.enable = false;
    bar.upCap.color = vec4(1);
    bar.upCap.rate = 0.01;
    bar.upCap.elasticity = .6;
    bar.upCap.launchVelocity = 1.1;
    bar.upCap.acceleration = .3;
    bar.upCap.launchFlingMultiplier = 1.2;
    bar.upCap.size.x = bar.size.y;
    bar.upCap.size.y = 2.;
    bar.upCap.offset = vec2(0);

    bar.innerSoftness = vec3(1);
    bar.outerSoftness = vec3(1, 1, 1);
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
    glow.intensity = 4.5;
    glow.directions = 16.0;
    glow.quality = 6.0;
    glow.color = vec4(0.2392, 0.0745, 0.6196, 1.0);
    glow.brightnessOffset = .2;
    glow.lightStrength = .7;
}

void setRotate0(inout Rotate rotate)
{
#define tstWt 1.3
#define tW2 tstWt* tstWt
#define tEW exp(-tW2)
#define dT (1. - texture(audioR, .06).x)

    rotate.angle = (-20 * abs(sin(1.5 * (clamp(((exp(-dT * dT * tW2) - tEW) * (tW2)) - .01, 0.1, .8)))));
}
