#include ":linear/structs.glsl"
#include ":utils/colors.glsl"
#include ":utils/post-processing/glow/structs.glsl"

#define coordinateRotation 0
#define fragmentWidth 12

#define leftPadding 10
#define rightPadding 10

#define visualiserDirections 2
#define visualiserMode 1

uniform float pwLength = 0;
uniform float pwEdits = 0;

void init()
{
    bar.audio.multiplier = 600;
    audioSettings.combineChannels = 1;
    bar.downCap.enable = true;
    // audioSettings.mode = 0;
}

void audioFetch(inout float fetchedAudio, float n, float lastN)
{

#define W2 9.3
#define EW exp(-W2)

    float d = 1. - fetchedAudio;

    fetchedAudio = ((exp(-d * d * W2) - EW));
    fetchedAudio *= 1. - step(abs(fetchedAudio), .00005);
}

void setOffsets(float direction, inout vec2 particleOffset, inout vec2 barOffset, inout vec2 barSizeOffset,
                vec2 barAudio, vec2 particleAudio, float xCoordinate, float n, float lastN)
{
}

void setBarProps()
{
    float maxAudioVal = -1;
    float maxAudioIndex = 0;

    // Dynamic Phase like After Effects!
    // for (float i = 0; i <= bar.fragment.lastN; i++)
    // {
    //     float newMaxAudioVal = max(maxAudioVal, texture(audioR, i / bar.fragment.lastN).x);
    //     maxAudioIndex = (newMaxAudioVal == maxAudioVal ? maxAudioIndex : i);
    //     maxAudioVal = newMaxAudioVal;
    // }

    float jitter = fract(sin(dot(vec2(bar.fragment.n, mod(pwEdits, 1024.)), vec2(12.9898, 78.233))) * 43758.5453);

    bar.color = interpolateHue(vec4(0.4627, 0.9294, 1.0, 0.6), .4, bar.fragment.n - maxAudioIndex + jitter * 8.,
                               bar.fragment.lastN);

    bar.color = mix(bar.color, vec4(1),
                    clamp(14 * (bar.audio.current.x + bar.audio.current.y) / (2. * bar.audio.multiplier), 0., 1.));

    bar.size = vec3(0, 10, 0);

    bar.innerSoftness = vec3(2);
    bar.innerSoftness -= clamp(13 * (bar.audio.current.x + bar.audio.current.y) / (2. * bar.audio.multiplier), 0., 2.);
    bar.outerSoftness = vec3(2);
    bar.outerSoftness -= clamp(39 * (bar.audio.current.x + bar.audio.current.y) / (2. * bar.audio.multiplier), 0., 1.);
    bar.type = 1;

    bar.borderSize = vec3(1);
    bar.borderColor = vec4(1.0, 1.0, 1.0, 1.0);
}
void setCapProps()
{
    bar.downCap.color =
        interpolateHue(vec4(0.4627, 0.9294, 1.0, 1.0), .4, bar.fragment.lastN - bar.fragment.n, bar.fragment.lastN);

    // bar.downCap.color = vec4(1);
    bar.downCap.size = vec2(7, 2);
    bar.downCap.softness = vec3(1, .1, 1);
    bar.downCap.acceleration = .1;

    // bar.downCap.audio.xy = vec2(3);
}

void setProps()
{

    float shown = min(pwLength, bar.fragment.lastN + 1.);

    if (bar.fragment.n >= shown)
    {
        bar.downCap.audio.xy = vec2(23);
        return;
    }

    setBarProps();

    setCapProps();
}

void setParticleDownProps()
{
}

void modifySDFs()
{
}

void setGlow0(inout Glow glow)
{
    glow.color = vec4(0.0, 0.0, 0.0, 1.0);

    glow.intensity = 2.;
    glow.directions = 16;
    glow.quality = 7;
    glow.size = vec2(12);
}