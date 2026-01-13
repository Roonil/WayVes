#include "angular/structs.glsl"
#include "utils/post-processing/glow/structs.glsl"

#define coordinateRotation 0.

#define audioRadialHeight 80

#define fragmentAngle 120.

#define centerCoords vec2(r_resolution.x / 2., r_resolution.y / 2.)

#define visualiserMode 0
#define visualiserDirections 0

#define circleRadius 12
#define circleBorderSize 0

#define audioModifier (.65 + 1.1 * texture(audioR, .067).x)

void init()
{
    particle.connector.left.enable = 1;
    particle.connector.right.enable = 1;
    circle.angleOffset = -110.;
}

void audioFetch(inout float fetchedAudio, float n, float lastN)
{
}

void setOffsets(float direction, inout vec2 particleOffset, inout vec2 barOffset, inout vec2 barSizeOffset, vec2 barAudio, vec2 particleAudio, float xCoordinate, float n, float lastN)
{
}

void setProps()
{

    circle.radius = circleRadius;
    circle.borderSize = circleBorderSize;

    particle.connector.left.height = 2.3;
    particle.connector.left.borderSize = 1.;

    particle.connector.left.color = vec4(1.0, 0.6196, 0.6196, clamp(audioModifier - .8, 0, 1)) * 1.4 * audioModifier;
    particle.connector.left.borderColor = vec4(1.0, 1.0, 1.0, 1.0) * 1.5 * audioModifier;
    particle.connector.left.innerSoftness = (2.5);
    particle.connector.left.outerSoftness = (4.5);
    particle.connector.jointMode = 1;

    particle.connector.right = particle.connector.left;
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

    glow.size = 7;
    glow.intensity = 1.5;
    glow.directions = 6.0;
    glow.quality = 8.0;
    glow.color = vec4(0.4039, 0.0667, 0.0667, 1.0);
    glow.brightnessOffset = .2;
    glow.lightStrength = .7;
}
