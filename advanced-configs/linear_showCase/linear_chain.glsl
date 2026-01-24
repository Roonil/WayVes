#include "linear/structs.glsl"
#include "linear/zOrders.glsl"
#include "utils/post-processing/glow/structs.glsl"

#define coordinateRotation 0.

#define fragmentWidth 50
#define leftPadding 0
#define rightPadding 0

#define visualiserDirections 0
#define visualiserMode 0

void init()
{

    audioSettings.mode = 1;
    bar.audio.multiplier = 102;
    particle.audio.multiplier = 102;

    // Enabling particle connectors
    particle.connector.left.enable = 1;
    particle.connector.right.enable = 1;

    audioSettings.combineChannels = 0;
}

void audioFetch(inout float fetchedAudio, float n, float lastN)
{
}

void setOffsets(float direction, inout vec2 particleOffset, inout vec2 barOffset, inout vec2 barSizeOffset, vec2 barAudio, vec2 particleAudio, float xCoordinate, float n, float lastN)
{
    // X-axis displacement for particles based on the audio.
    particleOffset.x += 6 * sin(particleAudio.y * .4);
}

void setConnectorBeam()
{
    // Width and height of the beam
    float lightBeamWidth = 12;
    float lightBeamHeight = 120;

    // Represents the beam of "light" that goes ahead in time and gets displaced with audio. Between 0 and 1, because smoothstep's return value is confined to this range. Similar to a wave equation with phase (wt - kx)
    float lineLightBeam = (1. - smoothstep(0, lightBeamWidth, abs(mod(time, r_resolution.x) - particle.fragment.coords.x)));

    // We smooth out the edges when the beam starts at the first particle,
    if (bar.fragment.n == 0)
        // 0 when the beam is behind bar.fragment.centerCoords.x - 24, and 1 when it is ahead of bar.fragment.centerCoords.x * 2 - 24. The position is determined by the final parameter, that is bar.fragment.coords.x
        lineLightBeam *= smoothstep(bar.fragment.centerCoords.x - 24, bar.fragment.centerCoords.x * 2 - 24, bar.fragment.coords.x);

    // and when the beam stops at the last particle
    if (bar.fragment.n == bar.fragment.lastN)
        lineLightBeam *= 1 - smoothstep(bar.fragment.centerCoords.x - 24, bar.fragment.centerCoords.x + 12, bar.fragment.coords.x);

    // Mixing in the height and softnesses based on lineLightBeam value
    particle.connector.left.height = mix(particle.connector.left.height, lightBeamHeight * lineLightBeam, .3);

    particle.connector.left.innerSoftness = mix(particle.connector.left.innerSoftness, 15., lineLightBeam);
    particle.connector.left.outerSoftness = mix(particle.connector.left.outerSoftness, 10., lineLightBeam);
}

void setProps()
{

    particle.radius = 12;
    particle.borderSize = 7;

    particle.innerSoftness = 3;
    particle.outerSoftness = 3;

    particle.color = vec4(1.0, 0.9608, 0.7765, 1.0);
    particle.borderColor = vec4(0, 0, 0, 1);

    particle.connector.left.color = vec4(1.0, 1.0, 1.0, 0.615);
    particle.connector.left.borderColor = vec4(0.0, 0.0, 0.0, 1.0);

    particle.connector.jointMode = 2;

    particle.connector.left.innerSoftness = 3;
    particle.connector.left.outerSoftness = 3;

    particle.connector.left.height = 10;
    particle.connector.left.borderSize = 5;

    // Uncomment to enable connectorBeam that goes forward with time, and wraps around after a while. Similar to linear_lights
    // setConnectorBeam();

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
