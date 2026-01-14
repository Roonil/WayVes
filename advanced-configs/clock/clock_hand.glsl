#include "angular/structs.glsl"
#include "utils/colors.glsl"

#define coordinateRotation 0.

#define isMinute 1

#define fragmentAngle 360.

#define visualiserMode 1
#define visualiserDirections 0

uniform float handAngle = 0;

void init()
{

    bar.audio.multiplier = 120;
    circle.radius = 50;
    circle.angleOffset = handAngle;
}

void audioFetch(inout float fetchedAudio, float n, float lastN)
{
}

void setOffsets(float direction, inout vec2 particleOffset, inout vec2 barOffset, inout vec2 barSizeOffset, vec2 barAudio, vec2 particleAudio, float xCoordinate, float n, float lastN)
{
}

void setProps()
{

    bar.color = mix(vec4(1.0, 0.0, 0.0, 1.0), vec4(0, 0, 0, 1), 1. - isMinute);
    bar.borderColor = mix(vec4(0.0, 0.0, 0.0, 1.0), vec4(0.9529, 0.9529, 0.9529, 1.0), 1. - isMinute);
    bar.borderSize = vec3(0, 0.75, 0);

    bar.size = vec3(0, 4.5, 80) + vec3(0, 0, mix(-12, 8, isMinute));
    bar.type = 2;
    bar.offset.y = 8 - mix(10, -15, isMinute);
    bar.innerSoftness = vec3(0, 3., 0);
    bar.outerSoftness = vec3(0, 2., 0);

    particle.radius = 7;
    particle.borderSize = 1.5;
    particle.innerSoftness = 2.5;
    particle.outerSoftness = 1.5;
    particle.borderColor = vec4(0.2667, 0.0, 1.0, 1.0);
    particle.color = mix(vec4(1.0, 1.0, 1.0, 1.0), vec4(0.3098, 0.0, 0.7137, 1.0), 2 * bar.audio.current.y / bar.audio.multiplier);
    particle.offset.y = -circle.radius;
}

void setParticleDownProps()
{
}

void modifySDFs()
{
}
