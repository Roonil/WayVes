#include ":angular/structs.glsl"
#include ":matugenColors/colors.glsl"
#include ":utils/post-processing/glow/structs.glsl"

#define coordinateRotation 0.

#define fragmentAngle .5

#define visualiserMode 2

#define visualiserDirections 0

void init()
{
    particle.audio.multiplier = 160;
    circle.center = vec2(0);

    circle.radius = int(r_resolution.x / 2) + 150;
    circle.angleOffset = -90;
    circle.maxAngle = 90;

    particle.connector.left.enable = 1;
    particle.connector.right.enable = 1;
}

void audioFetch(inout float fetchedAudio, float n, float lastN)
{
}

void setOffsets(float direction, inout vec2 particleOffset, inout vec2 barOffset, inout vec2 barSizeOffset,
                vec2 barAudio, vec2 particleAudio, float xCoordinate, float n, float lastN)
{
    particleOffset.y += 112 * texture(audioR, .1).x * sin(n * .18);
}

void setProps()
{
    if (bar.fragment.n == 0 || bar.fragment.n == bar.fragment.lastN)
        bar.color = matugenColors.outline;

    bar.size = vec3(162, 1, 82);

    bar.innerSoftness = vec3(1.5);

    if (bar.fragment.n == 0 || bar.fragment.n == bar.fragment.lastN)
        particle.radius = 2;

    particle.color = matugenColors.outline;

    particle.connector.left.height = 3;
    particle.connector.left.color = matugenColors.outline;

    particle.connector.left.innerSoftness = 12;

    particle.connector.right = particle.connector.left;
    particle.connector.jointMode = 1;
}

void setParticleDownProps()
{
}

void modifySDFs()
{
}
