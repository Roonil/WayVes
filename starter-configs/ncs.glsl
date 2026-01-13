#include "ncs/structs.glsl"

#include ":utils/post-processing/glow/structs.glsl"

#define colorTracking 0

void init()
{
    audio.multiplier = 6.4;
    audio.bassMultiplier = .5263 * resolution.x;
}

void setProps()
{

    particle.color = vec4(0.0353, 0.5216, 0.9725, 0.3);
    particle.size = 3;

    particle.feather = 1;

    particle.colorIntensityAddStrength = 0.38;
    particle.antiAlias = 8.5;

    fractalField.octaveMultiplier = 0.25;
    fractalField.octaveScale = 1.0;
    fractalField.complexity = 3;
    fractalField.fScale = 9.473;
    fractalField.gamma = 1.0;
    fractalField.minVal = -5.0;
    fractalField.maxVal = 5.0;

    fractalField.flows = vec4(0, 3.8, 0, 1.3);
    fractalField.displacements = vec3(.3884 * resolution.x, .3884 * resolution.x - 20, .3884 * resolution.x - 5);

    sphere.radius = .7236 * resolution.x;
    sphere.feather = 0.45;
}

void modifyNoiseCoordinates(inout vec4 coords)
{
}

void setPropsWithNoise()
{
}

void modifySphericalDisplacement()
{
}

void setGlow0(inout Glow glow)
{
    glow.blendMode = 1;
    glow.mixAlpha = 1;
    glow.intensity = 1.0;
    glow.size = 18;
    glow.directions = 16.0;
    glow.quality = 6.0;
    glow.color = vec4(0.0275, 0.0392, 0.6471, 1.0);
    glow.brightnessOffset = .0;
    glow.lightStrength = .5;
}
