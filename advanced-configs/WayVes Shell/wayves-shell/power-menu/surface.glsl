#include ":angular/structs.glsl"
#include ":matugenColors/colors.glsl"
#include ":utils/post-processing/glow/structs.glsl"

#define coordinateRotation 0.

#define fragmentAngle 360. / 7.

#define visualiserMode 1

#define visualiserDirections 0

#define isPotato 0.

void init()
{
    circle.radius = int(r_resolution.x / 2) - 63;
    circle.angleOffset = -40 + sign(abs(1. - isPotato)) * time * .14;
}

void audioFetch(inout float fetchedAudio, float n, float lastN)
{
}

void setOffsets(float direction, inout vec2 particleOffset, inout vec2 barOffset, inout vec2 barSizeOffset,
                vec2 barAudio, vec2 particleAudio, float xCoordinate, float n, float lastN)
{
}

void setCircleProps()
{
    circle.color = vec4(matugenColors.surface.xyz, .75);
    circle.innerSoftness = 2;
    circle.outerSoftness = 2;

    circle.borderSize = 3;
    circle.borderColor = vec4(matugenColors.outline.xyz, 1);
}

void setBarProps()
{
    if (bar.fragment.n == 6 || bar.fragment.n == 3)
        bar.borderSize = vec3(8, 0, 0);

    bar.size = vec3(0, fragmentAngle - 2, 20);
    bar.offset.y -= 42.5;

    bar.color = vec4(matugenColors.primaryContainer.xyz, .46);

    bar.innerSoftness = vec3(2, .3, 2);
    bar.outerSoftness = vec3(3, 0.3, 3);

    bar.borderColor = matugenColors.outline;
}

void setProps()
{
    setCircleProps();
    setBarProps();
}

void setParticleDownProps()
{
}

void modifySDFs()
{
}
