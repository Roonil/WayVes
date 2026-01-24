#include "linear/structs.glsl"
#include "linear/zOrders.glsl"
#include "utils/colors.glsl"
#include "utils/post-processing/glow/structs.glsl"

#define coordinateRotation 0.

// Small fragmentWidth to compactify the waves
#define fragmentWidth 3
#define leftPadding 15
#define rightPadding 10

#define visualiserMode 0

// isSmall = 1 represents when the wave is on the desktop, and 0 when it is on the top.
uniform float isSmall = 1;

// We want the visuals to go downwards when the visualiser is at top.
#define visualiserDirections (1 - isSmall)

void init()
{

    audioSettings.mode = 1;

    // Higher multiplier when on the desktop
    particle.audio.multiplier = mix(50, 450, isSmall);

    // Enabling connectors that represent waves in this case
    particle.connector.left.enable = 1;
    particle.connector.right.enable = 1;

    audioSettings.combineChannels = 0;
}

void audioFetch(inout float fetchedAudio, float n, float lastN)
{
// Lower audio sensitivity when on the desktop
#define W2 mix(4.3, 12.3, isSmall)
#define EW exp(-W2)

    float d = 1. - fetchedAudio;

    fetchedAudio = ((exp(-d * d * W2) - EW));
    fetchedAudio *= mix(1., 1. - step(abs(fetchedAudio), .005), 1.);

    // Alternates the directions of the waves when on the desktop, giving the analog signal look
    fetchedAudio *= mix(1., sign(mod(n, 2) * 2 - 1), isSmall);
}

void setOffsets(float direction, inout vec2 particleOffset, inout vec2 barOffset, inout vec2 barSizeOffset, vec2 barAudio, vec2 particleAudio, float xCoordinate, float n, float lastN)
{
}

void setProps()
{
    // Height and borderSize for connectors
    particle.connector.left.height = 1.8;
    particle.connector.left.borderSize = 1.3;

    // Inner and outer softnesses for the connectors
    particle.connector.left.innerSoftness = 1.3;
    particle.connector.left.outerSoftness = 1.5;

    // Hue interpolation that changes with audio value sampled at 0.3 from the Right Audio Channel.
    particle.connector.left.color = interpolateHue(mix(vec4(0.0, 0.8353, 1.0, 1.0), vec4(1.0, 0.051, 0.9529, 1.0), 3 * texture(audioR, .3).x), .5, particle.fragment.coords.x, r_resolution.x);
    particle.connector.left.borderColor = vec4(0.2588, 0.2588, 0.2588, 1.0);

    // jointMode = 1 to ensure a smooth and continuous look.
    particle.connector.jointMode = 1;

    // Copying the properties of the left connector to the right connector.
    particle.connector.right = particle.connector.left;
}

void setParticleDownProps()
{
}

void modifySDFs()
{
}

void setGlow0(inout Glow glow)
{
    glow.intensity = 3.5;
    glow.color = vec4(0.0, 0.0392, 0.051, 1.0);
    glow.directions = 16;
    glow.blendMode = 0;

    glow.mixAlpha = 1;
    glow.size = 13.5;
    glow.quality = 6;
    glow.brightnessOffset = .4;
    glow.lightStrength = .5;
}