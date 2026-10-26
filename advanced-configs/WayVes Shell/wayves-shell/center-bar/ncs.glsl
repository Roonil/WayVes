#include ":ncs/structs.glsl"

#include ":matugenColors/colors.glsl"
#include ":utils/post-processing/glow/structs.glsl"

#define colorTracking 0

vec4 color = vec4(matugenColors.primary.xyz, .15);

void init()
{
    audio.multiplier = 6.4;
    audio.bassMultiplier = .3 * resolution.x;
    baseForm.numParticles.xy = vec2(180);
}

void setProps()
{

    particle.color = color;
    particle.size = 3;

    particle.feather = 1;

    particle.colorIntensityAddStrength = 0.4;
    particle.antiAlias = 4.5;

    fractalField.octaveMultiplier = 0.25;
    fractalField.octaveScale = 1.0;
    fractalField.complexity = 3;
    fractalField.fScale = 10 * 380. / resolution.x;
    fractalField.gamma = 1.0;
    fractalField.minVal = -5.0;
    fractalField.maxVal = 5.0;
    fractalField.constantNoiseMultiplier = 0;

    fractalField.flows = vec4(0, 3.8, 0, 1.8) * resolution.x / 380.;
    fractalField.displacements = vec3(.3884 * resolution.x, .3884 * resolution.x - 20, .3884 * resolution.x - 5);

    sphere.radius = .73 * resolution.x;
    sphere.feather = 0.5;

    baseForm.rotations = rotateY(30);
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
    glow.intensity = .50;
    glow.size = vec2(18);
    glow.directions = 16.0;
    glow.quality = 6.0;
    glow.color = vec4(color.xyz / 4, color.w);
    glow.brightnessOffset = .0;
    glow.lightStrength = .5;
}
