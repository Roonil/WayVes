#include ":angular/structs.glsl"
#include ":matugenColors/colors.glsl"

#include ":utils/post-processing/glow/structs.glsl"

#define coordinateRotation 0.

#define fragmentAngle 1.5

#define visualiserMode 0

#define visualiserDirections 0

void init()
{
    particle.audio.multiplier = 1;
    circle.radius = int(r_resolution.x / 2) - 13;
    circle.angleOffset = -90 - time * .15;
    circle.maxAngle = 270;
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
    particle.radius = 2;
    particle.color = matugenColors.outline;

    particle.innerSoftness = 2;
    particle.color = mix(particle.color, vec4(0.0, 0.0, 0.0, 1.0), 2 * particle.audio.current.y);

    particle.borderColor = matugenColors.outline;

    particle.outerSoftness = .75;
    particle.radius += 2 * particle.audio.current.y;

    particle.audio.current = vec2(0);
}

void setParticleDownProps()
{
}

void modifySDFs()
{
}
