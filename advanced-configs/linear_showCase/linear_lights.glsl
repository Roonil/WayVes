#include "linear/structs.glsl"
#include "linear/zOrders.glsl"
#include "utils/post-processing/glow/structs.glsl"

// Horizontal layout
#define coordinateRotation -90.

#define fragmentWidth 100
#define leftPadding 90
#define rightPadding 90

#define visualiserDirections 0
#define visualiserMode 2

void init()
{

    audioSettings.mode = 1;

    bar.audio.multiplier = 102;
    particle.audio.multiplier = 102;

    // Enabling particle connectors
    particle.connector.left.enable = 1;
    particle.connector.right.enable = 1;
}

void audioFetch(inout float fetchedAudio, float n, float lastN)
{
}

void setOffsets(float direction, inout vec2 particleOffset, inout vec2 barOffset, inout vec2 barSizeOffset, vec2 barAudio, vec2 particleAudio, float xCoordinate, float n, float lastN)
{

    // Setting individual offsets for each bar and particle, distinguished by their number (n)
    if (n == 0) {
        particleOffset.y -= 20;
        barOffset.y -= 10;
        barSizeOffset.x -= 10;
    }

    if (n == 1) {
        particleOffset.x -= 10;
        barOffset.x -= 10;
        barOffset.y -= 1;
    }
    if (n == 2) {
        particleOffset.x += 13;
        barOffset.x += 13;
        barOffset.y -= 1;
    }

    if (n == 3) {
        particleOffset.x += 10;
        barOffset.x += 10;
        barSizeOffset.x += 10;
        barOffset.y -= 25;
        particleOffset.y -= 20;
    }
}

void setProps()
{

    // Bars with size 100 from top and 10 wide.
    bar.size = vec3(100., 10., 10);

    // 3 pixels wide border from left and right.
    bar.borderSize = vec3(0, 3, 0);

    // Colors for bars and their borders
    bar.color = (vec4(0.2039, 0.0157, 0.349, 1.0));
    bar.borderColor = vec4(0.451, 0.3255, 0.5294, 0.396);

    // Inner and outer softnesses for the bars
    bar.outerSoftness = vec3(.4, .4, .4);
    bar.innerSoftness = vec3(.3, .3, .3);

    // Inner softness from the top increases drastically with audio sampled at 0.1 from the Right Audio Texture
    bar.innerSoftness.x += 225 * texture(audioR, .1).x;

    // Global bar offset.
    bar.offset.y = 40 - r_resolution.y / 2.;

    // Radius of the particle
    particle.radius = 12;

    // Very large borderSize
    particle.borderSize = 34;

    // Inner softness for the particles
    particle.innerSoftness = 3;

    // Very large outer softness for the particles to counter borderSize. Results in a soft halo around the particles
    particle.outerSoftness = 120;

    // Global particle offset
    particle.offset.y -= r_resolution.y / 2. - 140;

    // Colors for particles and their borders
    particle.color = vec4(1.0, 0.9608, 0.7765, 1.0);
    particle.borderColor = vec4(1);

    // Connector colors
    particle.connector.left.color = vec4(0.7216, 0.6863, 0.5961, 0.533);
    particle.connector.left.borderColor = vec4(0.0, 0.0, 0.0, 1.0);

    // Connector jointMode. 1 means a smooth, continuous segment when encountering the "merger" of the connectors at the particle's center
    particle.connector.jointMode = 1;

    // Softness for inner and outer edges of the connectors
    particle.connector.left.innerSoftness = 6;
    particle.connector.left.outerSoftness = 5;

    // Height and borderSize for connectors.
    particle.connector.left.height = 2;
    particle.connector.left.borderSize = 1;

    // audio sampled at 0.1 from the Right Audio Texture
    float lowFreqAudio = texture(audioR, 0.1).x;
    float lightBeamWidth = 6;
    float lightBeamHeight = 120;

    // Represents the beam of "light" that goes ahead in time and gets displaced with audio. Between 0 and 1, because smoothstep's return value is confined to this range. Similar to a wave equation with phase (wt - kx)
    float lineLightBeam = (1. - smoothstep(0, lightBeamWidth + 10 * lowFreqAudio, abs(mod(.5 * time, r_resolution.x) - particle.fragment.coords.x + ((200 * lowFreqAudio)))));

    // We "mix" in the height of the connector based on the lineLightBeam value
    particle.connector.left.height = mix(particle.connector.left.height, lightBeamHeight * lineLightBeam, texture(audioR, particle.fragment.coords.x / r_resolution.x).x);

    // Similarly, we increase the inner and outer softness values based on lineLightBeam value.
    particle.connector.left.innerSoftness = mix(particle.connector.left.innerSoftness, 56., (1. - lineLightBeam));
    particle.connector.left.outerSoftness = mix(particle.connector.left.outerSoftness, 9., lowFreqAudio * (1. - lineLightBeam));

    // For the very first particle, ensure that connector does not show when it is "behind" the particle. This is needed because as the connector's height increases with audio, the connector is supposed to appear "behind" the particle simply because it has a large size. Try disabling lightBeam and increasing connectorHeight to observe this.
    if (particle.fragment.n == 0 && (particle.fragment.coords.x < particle.fragment.centerCoords.x)) {
        particle.connector.left.height = 0;
    }

    // Similarly, we ensure the same for the last particle, ensuring that the connector does not draw "after" the particle.
    else if (particle.fragment.n == particle.fragment.lastN && particle.fragment.coords.x > particle.fragment.centerCoords.x) {
        particle.connector.left.height = 0;
    }

    // Copy the left connector's configuration to right, since we want them to appear continuous
    particle.connector.right = particle.connector.left;
}

void setParticleDownProps()
{
}

void modifySDFs()
{
    // See the wiki for more details.
    // Sets the Z-Index for objects within the upper particle group, such that the particles appear at the top of the connectors (the inner body of the particles as well as their borders)
    setParticleUpGroupZIndex(2, 3, 4, 5, 6, 0, 1);
    setParticleUpGroupCBM(0, 0, 0, 0, 0, 0, 0);
    applyZOrders();
}

// First glow pass. Gives the faded out glow effect from the particle horizontally.
void setGlow0(inout Glow glow)
{
    glow.intensity = 0.15;
    glow.color = vec4(0.298, 0.2549, 0.0118, 1.0);
    glow.directions = 2;
    glow.blendMode = 0;
    glow.mixAlpha = 0;
    glow.size = 80;
    glow.quality = 12;
    glow.brightnessOffset = .0;

    // Adjusting lightStrength so the bottom of the bars appears dimmer
    glow.lightStrength = mix(.4, 1. - smoothstep(130, 280, glow.coords.x), 1. - smoothstep(0, 130, glow.coords.x));
}

// Second glow pass. Very expensive because of the quality and directions. Gives the spherical halo much more prominence.
void setGlow1(inout Glow glow)
{
    // Intensity changes with audio, so the "lights" appear to grow brighter with audio sampled at 0.4 from the Right Audio Channel.
    glow.intensity = clamp(.5 + 3 * texture(audioR, .4).x, 0, 2);

    glow.color = vec4(0.8196, 0.7059, 0.051, 1.0);
    glow.directions = 64;
    glow.blendMode = 0;
    glow.mixAlpha = 0;
    glow.size = 60.5;
    glow.quality = 12;
    glow.brightnessOffset = .4;

    // Adjusting lightStrength so it increases gradually as we go from the bottom of the bars and towards the "lights" or particles
    glow.lightStrength = smoothstep(0., 120., glow.coords.x);
}