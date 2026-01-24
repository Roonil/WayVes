#include "linear/structs.glsl"
#include "utils/post-processing/glow/structs.glsl"

// Begin User-defined Variables

// --------- Uniforms ---------

// mergeBars can be used to toggle merging bars
uniform int mergeBars = 1;

// isLarge is the runtime attribute that we can modify to change the bars' size, multiplier and softnesses when the window size becomes small
uniform float isLarge = 1;

// --------- Bar Steps (that appear in the background, can be made uniforms as well) ---------

// Width of the step over which a box appears to "rest"
float stepWidth = 7;

// Width of the step over which a box appears to "rest"
float stepHeight = 2;

// The "chunks" to divide audio into. Lower values provide more "jumps"
float audioChunkSize = 5;

// Distance between the steps
float stepSpacing = 30;

// Number of steps
float stepLevels = 6;

// End User-defined Variables

// Horizontal when isLarge=1, and Vertical when isLarge=0
#define coordinateRotation mix(0., 90., isLarge)

// Smaller window has fragmentWidth 12, while larger window has 23
#define fragmentWidth int(mix(12, 23, isLarge))
#define leftPadding 0
#define rightPadding 0

#define visualiserDirections 1.
#define visualiserMode 1

void init()
{

    audioSettings.mode = 1;

    // Smaller window width gets audio.multiplier of 50, while larger one gets 102
    bar.audio.multiplier = mix(50, 102, isLarge);

    // Merge bars based on the uniform mergeBars
    bar.mergeLeftBar = mergeBars;
    bar.mergeRightBar = mergeBars;

    audioSettings.combineChannels = 0;
}

void audioFetch(inout float fetchedAudio, float n, float lastN)
{

    // Acts as a smoothing filter between higher and lower audio values. Increase W2 to make audio reaction less sensitive.

#define W2 4.3
#define EW exp(-W2)

    float d = 1. - fetchedAudio;

    fetchedAudio = ((exp(-d * d * W2) - EW));
    fetchedAudio *= 1. - step(abs(fetchedAudio), .005);
}

void setOffsets(float direction, inout vec2 particleOffset, inout vec2 barOffset, inout vec2 barSizeOffset, vec2 barAudio, vec2 particleAudio, float xCoordinate, float n, float lastN)
{
    // Makes the bars move as a whole based on the audio, meaning the "bottom" of the bars gets moved up with audio
    // sign(2 * isLarge - 1) => -1 when isLarge = 0, and 1 when isLarge = 1. Needed because we switch directions based on the layout
    barOffset.y = sign(2 * isLarge - 1) * int((barAudio.x + barAudio.y) / audioChunkSize) * stepSpacing;
}

void setProps()
{

    // Setting size and borderSize for bars based on the layout
    // Smaller window -> bars are 9 pixels high (from top) and 5 pixels wide.
    // Larger window -> bars are 12 pixels high (from top) and 10 pixels wide.
    bar.size = mix(vec3(9., 5., 0), vec3(12., 10., 0), isLarge);

    // Border Size for the bars. 2 at the top, 1 at left and right side each, and 1 at the bottom.
    // Note that the top and bottom borders do not add to the height of the bar as specified by bar.size, but the left and right borders do.
    bar.borderSize = vec3(2, 1, 1);

    // Softness for outer and inner edges
    bar.outerSoftness = vec3(3, .1, 1);
    bar.innerSoftness = vec3(1, .3, 1);

    // Color for bars and their borders.
    bar.color = (vec4(0.302, 0.0, 0.702, 0.301));
    bar.borderColor = vec4(1.0, 0.6706, 0.8353, 1.0);

    // Different offsets based on window layout, because we switch directions
    bar.offset.y = mix(r_resolution.y / 2. - bar.size.x, -r_resolution.y / 2., isLarge);

    // Doesn't display the steps when the window size is small
    bar.bgColor = mix(vec4(0), vec4(0.0, 1.0, 0.3843, 1.0), isLarge);

    // Allow the current bar's audio to affect the step opacity.
    sdfs[BAR_BG_SDF] *= bar.audio.current.x / bar.audio.multiplier;

    // Setting audio values to zero, because we don't want to modify the length of the bars.
    bar.audio.current = vec2(0);
    bar.audio.prev = vec2(0);
    bar.audio.next = vec2(0);
}

void setParticleDownProps()
{
}

void modifySDFs()
{
    // We divide the background in `stepSpacing` steps
    float steps = mod(bar.fragment.coords.y + stepHeight + 1, stepSpacing);

    // Ensure that only the background pixels with `stepHeight` height and `stepLevels` number of steps get shown
    if (steps >= stepHeight || bar.fragment.coords.y > stepLevels * stepSpacing)
        sdfs[BAR_BG_SDF] = 0;

    // The width of the steps
    sdfs[BAR_BG_SDF] *= step(abs(bar.fragment.span), stepWidth);
}

void setGlow0(inout Glow glow)
{
    glow.intensity = 0.5;
    glow.color = vec4(0.0118, 0.0, 0.2, 1.0);
    glow.directions = 16;
    glow.blendMode = 0;
    glow.mixAlpha = 0;
    glow.size = 3.5;
    glow.quality = 3;
    glow.brightnessOffset = .4;
    glow.lightStrength = 1;
}
