#include "linear/structs.glsl"
#include "linear/zOrders.glsl"
#include "utils/post-processing/glow/structs.glsl"

// The single Bar appears horizontally rather than vertically
#define coordinateRotation 90.

// Defining fragmentWidth and paddings so that we only see 1 bar and 1 particle
#define fragmentWidth int(r_resolution.x / 2)
#define leftPadding int(r_resolution.x / 4)
#define rightPadding int(r_resolution.x / 4)

#define visualiserDirections 0

// For showing both particle and bars
uniform int visualiserMode = 2;

// Radius of the particle
uniform float particleRadius = 32;

void init()
{

    audioSettings.mode = 1;
    bar.audio.multiplier = 102;
    particle.audio.multiplier = 102;

    audioSettings.combineChannels = 0;
}

void audioFetch(inout float fetchedAudio, float n, float lastN)
{
}

void setOffsets(float direction, inout vec2 particleOffset, inout vec2 barOffset, inout vec2 barSizeOffset, vec2 barAudio, vec2 particleAudio, float xCoordinate, float n, float lastN)
{
    // Offseting bar so it appears slightly out-of-place with the particle
    barOffset.y += 90 - r_resolution.y / 2.;
    // Offsetting to accommodate for layout changes. 0 when visualiserMode = 0, and 70 when visualiserMode > 0
    particleOffset.y += mix(0, 70, sign(visualiserMode));
}

void setProps()
{
    // Rounded bar
    bar.type = 1;

    // Bar is 120 pixels high from the top, `fragmentWidth / 2.0` wide, and 10 pixels low from the bottom
    bar.size = vec3(120., fragmentWidth / 2., 10);

    // Border size for rounded bars is determined by borderSize.y
    bar.borderSize = vec3(2);

    // Inner and outer softnesses for bar
    bar.outerSoftness = vec3(2);
    bar.innerSoftness = vec3(2);

    // Bar color and Bar Border Color
    bar.color = (vec4(0.2039, 0.0157, 0.349, 0.143));
    bar.borderColor = mix(vec4(0.4667, 0.0, 0.1176, 0.921), vec4(1), particle.audio.current.y / bar.audio.multiplier);

    // particle radius changes with audio
    particle.radius = particleRadius + .2 * particle.audio.current.y;
    particle.borderSize = 4;

    // Particle inner softness changes with audio sampled at 0.1 from the Right Audio Texture
    particle.innerSoftness = 2 + 42 * texture(audioR, 0.1).x;
    particle.outerSoftness = 2;

    // Particle color changes with audio sampled at 0.5 from the Right Audio Texture
    particle.color = vec4(0.5373, 0.7412, 0.9922, 0.259) + vec4(0.0, 0.4824, 1.0, 0.45) * texture(audioR, 0.5).x;

    // Particle gets the same borderColor as the bar's borderColor
    particle.borderColor = bar.borderColor;

    // Setting audios to 0 as we don't want vertical displacements
    particle.audio.current = vec2(0);
    bar.audio.current = vec2(0);
}

void setParticleDownProps()
{
}

void modifySDFs()
{
    // Makes the portion of the bar that is behind the particle completely invisible
    // clamping ensures that the values stay between 0 and 1.
    sdfs[BAR_INNER_SDF] = clamp(sdfs[BAR_INNER_SDF] - sdfs[PARTICLE_UP_INNER_SDF], 0, 1);
    sdfs[BAR_INNER_SDF] = clamp(sdfs[BAR_INNER_SDF] - sdfs[PARTICLE_UP_OUTER_SDF], 0, 1);
    sdfs[BAR_OUTER_SDF] = clamp(sdfs[BAR_OUTER_SDF] - sdfs[PARTICLE_UP_INNER_SDF], 0, 1);
    sdfs[BAR_OUTER_SDF] = clamp(sdfs[BAR_OUTER_SDF] - sdfs[PARTICLE_UP_OUTER_SDF], 0, 1);
}

void setGlow0(inout Glow glow)
{
    glow.intensity = .5;
    glow.color = vec4(0.4588, 0.4588, 0.4588, 1.0);
    glow.directions = 16;
    glow.blendMode = 1;
    glow.mixAlpha = 0;

    // Glow size changes based on visualiserMode
    glow.size = mix(5., 18.5, float(sign(visualiserMode)));
    glow.quality = 6;

    // Makes the outer edges glow significantly more
    glow.brightnessOffset = .4;
    glow.lightStrength = 1;
}