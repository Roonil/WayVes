#include ":matugenColors/colors.glsl"
#include ":ncs/structs.glsl"

#include ":utils/post-processing/glow/structs.glsl"

#define colorTracking 0

vec4 color = matugenColors.primary;

void init()
{
    audio.multiplier = 0;
    audio.bassMultiplier = 0;
    baseForm.zSize = resolution.x;

    baseForm.numParticles.xyz = vec3(30, 10, 5);
}

void setProps()
{
    if (particle.position.z < 0)
        baseForm.scale = vec3(1 + (1. - particle.position.y / resolution.y) * 5, 2, 2);

    else
        baseForm.scale = vec3(1 + particle.position.y / resolution.y * 2, 2, 2);

    baseForm.scale *= 1.3;
    float signPos = step(resolution.y / 2., particle.position.y) * 2. - 1;

    // particle.position.y = signPos * resolution.y / 2. + mod(particle.position.y + signPos * time, resolution.y);
    baseForm.rotations = rotateX(40);
    // fractalField.noiseType = 1;
    // fractalField.displacementType = 1;
    particle.color = color;

    particle.size = 1;

    particle.feather = 1;

    particle.colorIntensityAddStrength = 0.1;
    particle.antiAlias = .5;

    particle.position.y = mod(particle.position.y - (particle.position.z < 0 ? 1. : -1) * time, resolution.y);

    fractalField.affectSize = 15;

    fractalField.affectOpacity = 15;
    fractalField.octaveMultiplier = 2.2;
    fractalField.octaveScale = 1.50;
    fractalField.complexity = 3;
    fractalField.fScale = 5.5;
    // fractalField.gamma = 1.20;
    // fractalField.minVal = -5.0;
    // fractalField.maxVal = 5.0;
    fractalField.constantNoiseMultiplier = .5;

    fractalField.flows = vec4(0, 0, 1.5, .0);
    fractalField.displacements = vec3(1.3884 * resolution.x, 1.3884 * resolution.x - 20, 1.3884 * resolution.x - 5);
    // fractalField.displacements = vec3(0, 220, 0);
}

void modifyNoiseCoordinates(inout vec4 coords)
{
    // if (coords.y > resolution.y / 2. && coords.z == 0)
    //     coords.y = resolution.y - coords.y;
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
    glow.intensity = 0.5;
    glow.size = vec2(10);
    glow.directions = 16.0;
    glow.quality = 6.0;
    glow.color = matugenColors.primary;
    glow.brightnessOffset = .4;
    glow.lightStrength = .5;
}
