#version 330 core

in vec2 r_resolution;
in vec4 r_ngl_FragCoord;

uniform vec2 resolution;

uniform sampler1D audioL;
uniform sampler1D audioR;

uniform float time;
uniform int audioLSize;
uniform int audioRSize;

out vec4 FragColor;

vec4 r_gl_FragCoord = vec4(floor(r_ngl_FragCoord.xy * r_resolution.xy) + .5, r_ngl_FragCoord.zw);

#include ":$CONFIGFILE"

// Adapted from Panon's Chain Shader by rbn42
// Licensed under GPL-3.0

void defaultAudioValues()
{
    audio.multiplier = 1;
    audio.value = vec2(0);
}

void defaultAudioSettingsValues()
{
    audioSettings.reverseLeft = 0;
    audioSettings.reverseRight = 0;
    audioSettings.mode = 1;
    audioSettings.combineChannels = 0;
}

void defaultChainValues()
{
    chain.index = 0;
    chain.interChannelDistance = 0;
    chain.heightRatio = 1.0;
    chain.strength = 0.175;
    chain.verticalColorDropExtent = .97;
    chain.channelLineHeight = 0;
    chain.radius = 30.;
    chain.density = 128;
    chain.color = vec4(0, 0, 1, 1);
}

float getAudioVal(float n, float lastN, float isLeftSide)
{

    float audioX = n / lastN;

    int combineChannels = audioSettings.combineChannels;

    float audioVal = clamp(isLeftSide + (combineChannels), 0, 1) * texture(audioL, mix(audioX, 1. - audioX, audioSettings.reverseLeft)).x;
    audioVal += clamp(1 - isLeftSide + (combineChannels), 0, 1) * texture(audioR, mix(audioX, 1. - audioX, audioSettings.reverseRight)).x;
    audioVal /= 1. + (combineChannels);

    audioFetch(audioVal, n, lastN);
    return audioVal;
}

float randomValue(vec2 co)
{
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    defaultAudioSettingsValues();
    defaultAudioValues();
    defaultChainValues();

    init();

    float height = r_gl_FragCoord.y / r_resolution.y;
    float topSelector = 1. - step(height, 0.5);

    height = abs(2 * height - 1);

    FragColor = vec4(0, 0, 0, 0);

    for (int j = 0; j < chain.density; j++) {
        float randomNumber = randomValue(r_gl_FragCoord.xy / r_resolution.xy + vec2(time / 60 / 60 / 24 / 10, j));
        float currentDistance = (2 * randomNumber - 1);

        float i = (2 * randomNumber - 1) * chain.radius / sqrt(chain.strength);

        chain.index = r_gl_FragCoord.x + i;

        float mode = (1. - audioSettings.mode);

        float isLeftSide = mode * step(chain.index, r_resolution.x / 2.);

        audio.value.x = audio.multiplier * getAudioVal(abs((mode)*r_resolution.x / 2. - chain.index), r_resolution.x / (1. + mode), isLeftSide + (1 - mode) * (1 - isLeftSide));
        audio.value.y = audio.multiplier * getAudioVal(abs((mode)*r_resolution.x / 2. - chain.index), r_resolution.x / (1. + mode), isLeftSide * mode);

        setProps();

        float currentHeight = height;

        float maxAudio = mix(audio.value.x, audio.value.y, topSelector);
        float maxHeight = r_resolution.y * chain.heightRatio;

        float heightTarget2 = maxHeight * maxAudio
            * exp(-currentDistance * currentDistance / chain.strength);
        heightTarget2 = max(chain.interChannelDistance, heightTarget2);
        float heightTarget1 = heightTarget2 - (1. - chain.verticalColorDropExtent) * maxHeight;

        FragColor
            += (step(heightTarget1, (currentHeight)*r_resolution.y))
            * (step((currentHeight)*r_resolution.y, max(chain.interChannelDistance + chain.channelLineHeight, heightTarget2)))
            * vec4(chain.color.xyz * chain.color.w, chain.color.w);
    }
}