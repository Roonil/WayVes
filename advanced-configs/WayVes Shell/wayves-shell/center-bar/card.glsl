#include ":linear/structs.glsl"
#include ":linear/zOrders.glsl"

#include ":utils/post-processing/glow/structs.glsl"

#include ":matugenColors/colors.glsl"

#define coordinateRotation 0

#define roundedRadius 20

#define fragmentWidth (10)

#define paddings int(mod(r_resolution.x, fragmentWidth) / 2.)

#define leftPadding (paddings + (roundedRadius / fragmentWidth + 1 + .5) * fragmentWidth)
#define rightPadding (paddings + (roundedRadius / fragmentWidth + 1 + .5) * fragmentWidth)

#define amplify 102.
#define weight 3.4

#define enableDecay true
#define enableConnectors false
#define alternateAmplitude false
#define combineAudioChannels false
#define visualiserDirections 1
#define visualiserMode 1
#define audioExponentiate true

vec4 barColor = vec4(matugenColors.primaryContainer.xyz, .15);
vec4 borderColor = matugenColors.outline;

void init()
{
    audioSettings.mode = 1;

    particle.connector.left.enable = 1;

    bar.audio.multiplier = amplify;
    bar.mergeLeftBar = 1;
    bar.mergeRightBar = 1;
    audioSettings.combineChannels = 0;
}
void audioFetch(inout float fetchedAudio, float n, float lastN)
{
#define W2 weight *weight
#define EW exp(-W2)
    float d = 1. - fetchedAudio;
    fetchedAudio = (alternateAmplitude ? sign(mod(int(n), 2) * 2 - 1) : 1) *
                   (!audioExponentiate ? fetchedAudio : (exp(-d * d * W2) - EW) * (W2));
    fetchedAudio *= 1. - step(abs(fetchedAudio), .005);
}
void setOffsets(float direction, inout vec2 particleOffset, inout vec2 barOffset, inout vec2 barSizeOffset,
                vec2 barAudio, vec2 particleAudio, float xCoordinate, float n, float lastN)
{
}

void setBarProps()
{
    float barHeight = r_resolution.y / 2. - 6;

    bar.size = vec3(barHeight, 12, barHeight);

    bar.borderSize = vec3(1);
    bar.size.y += texture(audioR, .1).x * 6;
    bar.borderSize += texture(audioR, .1).x * 6;

    bar.outerSoftness = vec3(2.75);
    bar.innerSoftness = vec3(2.);

    bar.outerSoftness += clamp(texture(audioR, .03).x * 12, 0., 30.);
    bar.innerSoftness += clamp(texture(audioR, .1).x * 12, 0., 10.);

    if (bar.fragment.n == 0 && bar.fragment.span <= 0)
    {
        bar.type = 1;

        bar.size.y = (roundedRadius * 2);
        bar.size.xz -= bar.size.y / 2. + bar.borderSize.y / 2.;
    }
    else if (bar.fragment.n == bar.fragment.lastN && bar.fragment.span > 0)
    {
        bar.type = 1;
        bar.size.y = (roundedRadius * 2);

        bar.size.xz -= bar.size.y / 2. + bar.borderSize.y / 2.;
    }

    bar.color = barColor;
    // bar.borderColor = borderColor;
    bar.borderColor = mix(mix(borderColor, bar.color, .5), vec4(1), .5);

    bar.audio.current = vec2(0);
}

void setProps()
{
    setBarProps();
    // vec4 color1 = matugenColors.sourceColor;
    // color1.w *= 80.;
    bar.color.w = mix(bar.color.w, bar.color.w * .05, (bar.fragment.coords.x) / r_resolution.x);
    bar.color.w = mix(bar.color.w, bar.color.w * .05, bar.fragment.coords.y / r_resolution.y);

    bar.color = mix(bar.color, vec4(matugenColors.sourceColor.xyz, .05),
                    clamp(abs((bar.fragment.coords.x) / r_resolution.x - 1.2 * texture(audioR, .1).x), 0., 1.));
}

void setParticleDownProps()
{
}

void modifySDFs()
{
    setLayerOffsets(0, 1, 2);
}

#define glowSize0 5.50
#define glowIntensity0 .8

void setGlow0(inout Glow glow)
{
    glow.intensity = 0.5 + clamp(texture(audioR, .1).x * 12, 0., 3.);

    glow.color = mix(vec4(matugenColors.primary.xyz, .15), vec4(matugenColors.primaryContainer.xyz, .15), .5);

    glow.directions = 16;
    glow.blendMode = 1;
    glow.mixAlpha = 1;
    glow.onTop = 0;
    glow.size = vec2(5.5 + texture(audioR, .1).x * 8);
    glow.quality = 3;
    glow.brightnessOffset = .5 + texture(audioR, .1).x * .5;
    glow.lightStrength = 1;
}
