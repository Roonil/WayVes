#include "linear/structs.glsl"
#include "utils/post-processing/glow/structs.glsl"

#define coordinateRotation 0

// isLarge is the runtime attribute that we can modify to change the bars' size, multiplier and softnesses when the window size becomes small
uniform float isLarge = 1;

// mergeBars can be used to toggle merging bars
uniform int mergeBars = 1;

// enable or disable color shift with time
uniform float timeShift = 1;

// Colors that are used as gradients for the bar
uniform vec4 bottomColor = vec4(0.2, 0.0667, 0.3255, 1.0);
uniform vec4 midColor = vec4(0.0, 1.0, 0.8824, 1.0);
uniform vec4 topColor = vec4(1.0, 0.0, 0.9176, 1.0);

// Smaller window has fragmentWidth 23, while larger window has 50
#define fragmentWidth int(mix(23, 40, isLarge))
#define leftPadding 0
#define rightPadding 0

// Smaller bars go down, while larger bars go up
#define visualiserDirections (1.0 - isLarge)
#define visualiserMode 1

void init()
{
    audioSettings.mode = 0;
    // Smaller window has multiplier 60, while larger window has 470
    bar.audio.multiplier = mix(60.0, 470.0, isLarge);

    // Set by mergeBars
    bar.mergeLeftBar = mergeBars;
    bar.mergeRightBar = mergeBars;
}

void audioFetch(inout float fetchedAudio, float n, float lastN)
{
}

void setOffsets(float direction, inout vec2 particleOffset, inout vec2 barOffset, inout vec2 barSizeOffset, vec2 barAudio, vec2 particleAudio, float xCoordinate, float n, float lastN)
{
}

// Helper function to return a smooth gradient based on height.
float getMixVal(float startValue, float endValue)
{
    return smoothstep(startValue * r_resolution.y, endValue * r_resolution.y, bar.fragment.coords.y);
}

void setProps()
{

    // Setting size and borderSize for bars based on the layout
    bar.size = vec3(0.0, mix(7.0, 18.0, isLarge), 0);
    bar.borderSize = mix(vec3(3, 5, 8), vec3(15, 12, 19), isLarge);

    // By default, everything is placed in the center vertically. We move the bars upwards or downwards to utilise the full window height
    bar.offset.y = -(isLarge * 2 - 1) * r_resolution.y / 2.0;

    // Smaller bars have less softness, while larger bars have more
    bar.outerSoftness = mix(vec3(1, 4, 8), vec3(13, 10, 19), isLarge);

    // gradient mixing for the colors
    bar.color = mix(
        mix(bottomColor,
            mix(bottomColor, topColor, abs(sin(time * 0.02))), timeShift),
        mix(
            mix(midColor, mix(midColor, bottomColor, abs(cos(time * 0.01))), timeShift),
            mix(topColor, mix(topColor, midColor, abs(cos(time * 0.005))), timeShift),
            getMixVal(0.2, 1)),
        getMixVal(0.0, 0.4));

    bar.borderColor = vec4(0, 0, 0, 1);
}

void setParticleDownProps()
{
}

void modifySDFs()
{
}