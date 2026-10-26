#include ":linear/structs.glsl"
#include ":matugenColors/colors.glsl"

uniform float panelOpacity = .65;

#define coordinateRotation 0
#define fragmentWidth 40

#define leftPadding 20
#define rightPadding 0

#define visualiserDirections 0
#define visualiserMode 1

void init()
{
    bar.audio.multiplier = r_resolution.y;
    bar.mergeLeftBar = 1;
    bar.mergeRightBar = 1;
    bar.upCap.enable = true;
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
    bar.size = vec3(0, fragmentWidth / 2, 0);

    bar.color = vec4(matugenColors.surface.xyz, panelOpacity);

    bar.offset.y = -r_resolution.y / 2. - 2;

    bar.upCap.color = vec4(matugenColors.outline.xyz, .5);
    bar.upCap.size = vec2(bar.size.y, 2);
    bar.upCap.softness = vec3(.5, 2, .5);
    bar.upCap.acceleration = .1;
}

void setParticleDownProps()
{
}

void modifySDFs()
{
}