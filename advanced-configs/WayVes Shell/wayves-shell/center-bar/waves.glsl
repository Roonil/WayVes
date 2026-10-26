#include ":linear/structs.glsl"
#include ":linear/zOrders.glsl"
#include ":matugenColors/colors.glsl"

#define coordinateRotation 0.
#define amplify 100

#define alphaMultiplier 1

vec4 barColor = matugenColors.primary * (alphaMultiplier);
vec4 capColor = matugenColors.primary;

#define fragmentWidth 1
#define enableDecay false

#define rightPadding 0
#define leftPadding 0

#define visualiserMode 1

#define visualiserDirections 0

void init()
{
    audioSettings.mode = 1;
    bar.audio.multiplier = amplify;
    bar.mergeLeftBar = 1;
    bar.mergeRightBar = 1;

    audioSettings.reverseRight = 1;
}

void audioFetch(inout float fetchedAudio, float n, float lastN)
{

#define W2 (4.3)
#define EW exp(-W2)

    float d = 1. - fetchedAudio;

    fetchedAudio = ((exp(-d * d * W2) - EW));
    fetchedAudio *= mix(1., 1. - step(abs(fetchedAudio), .005), 1.);
}

void setOffsets(float direction, inout vec2 particleOffset, inout vec2 barOffset, inout vec2 barSizeOffset,
                vec2 barAudio, vec2 particleAudio, float xCoordinate, float n, float lastN)
{
}

void setProps()
{
    bar.size = vec3(0, 1, 0);
    bar.color = barColor;

    bar.offset.y = -r_resolution.y / 2.;

    bar.innerSoftness = vec3(3, 3, 2);
    bar.outerSoftness = vec3(4, 3, 2);

    bar.upCap.enable = enableDecay;
    bar.upCap.color = capColor;
    bar.upCap.size = vec2(4, 3.5);
    bar.upCap.softness = vec3(2.5, .75, 2);

    bar.upCap.acceleration = .35;
    bar.upCap.offset.y = -1.5;
    bar.upCap.rate = 0;
    bar.upCap.launchVelocity = 0;
    bar.upCap.launchFlingMultiplier = 7;
}

void setParticleDownProps()
{
}

void modifySDFs()
{
    sdfs[BAR_UP_CAP_SDF] = clamp(sdfs[BAR_UP_CAP_SDF] * (1 - sdfs[BAR_INNER_SDF]), 0., 1.);
}
