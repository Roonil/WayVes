#include "ncs/structs.glsl"

#include ":utils/post-processing/glow/structs.glsl"

#define colorTracking 0

void init()
{
    audio.multiplier = 3;
    baseForm.type = 1;
    baseForm.numParticles = vec3(50, 50, 3);
}

void setProps()
{

    particle.color = vec4(1.0, 0.302, 0.6275, 0.139);
    particle.size = 1;

    baseForm.scale = vec3(3);

    particle.feather = 0.6;

    particle.colorIntensityAddStrength = 0.9;
    particle.antiAlias = 6;

    fractalField.octaveMultiplier = 0.2;

    fractalField.octaveScale = 2.0;
    fractalField.complexity = 4;
    fractalField.fScale = 17;
    fractalField.gamma = 1.0;
    fractalField.minVal = -5.0;
    fractalField.maxVal = 5.0;
    fractalField.constantNoiseMultiplier = .05;

    fractalField.flows = vec4(-0.966, 0.66, 0.966, .094);
    fractalField.displacements = vec3(-760, 400, -760);
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
    glow.blendMode = 0;
    glow.mixAlpha = 0;

    glow.size = 7;
    glow.directions = 16.0;
    glow.quality = 6.0;
    glow.color = vec4(0.5412, 0.0, 0.4235, 1);
    glow.brightnessOffset = .5;
    glow.lightStrength = .92;
}
