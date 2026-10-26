#include ":ncs/structs.glsl"

#include ":utils/colors.glsl"
#include ":utils/post-processing/glow/structs.glsl"

#define colorTracking 1

void init()
{
    audio.multiplier = 19;
    baseForm.numParticles = vec3(180, 3, 3);
    baseForm.zSize = resolution.x;
    baseForm.type = 1;

    baseForm.scale.xy = particle.position.y > resolution.y / 2. ? vec2(2.85) : vec2(2);
}

void setProps()
{

    particle.position.y += 300 * sign(particle.position.y - resolution.y / 2.) * 2. *
                           abs(resolution.y / 2. - particle.position.y) / resolution.y * texture(audioR, .1).x;

    particle.position.y -= 220;

    // particle.position.y = mod(particle.position.y + time * 2, resolution.y);

    particle.color = interpolateHue(vec4(0.1882, 0.6549, 0.5294, 1.0), .3, abs(particle.position.x - resolution.x / 2.),
                                    resolution.x / 2.);
    particle.color.w = mix(particle.color.w, particle.color.w * .5,
                           clamp(texture(audioR, particle.position.x / resolution.x).r, 0., 1.));

    particle.color = mix(particle.color, vec4(1.0, 0.0, 0.651, 1.0), abs(particle.position.z) / (2 * baseForm.zSize));

    particle.color.w = mix(particle.color.w, .3, 1. - particle.position.y / resolution.y);

    // particle.color =
    //     mix(particle.color,
    //         vec4(vec3(1) * 2 * abs(resolution.y / 2. - particle.position.y) / resolution.y, particle.color.w),
    //         texture(audioR, .1).x);
    particle.size = 3;
    fractalField.affectSize = 8 * (texture(audioR, particle.position.x / resolution.x).r);
    fractalField.affectOpacity = 1.5 * (texture(audioR, particle.position.x / resolution.x).r);

    fractalField.noiseType = 1;
    particle.feather = .5;
    baseForm.rotations = rotateZ(-28) * rotateY(90) * rotateX(-55) * rotateY(-10) * rotateX(20) * rotateY(time * .1);

    particle.colorIntensityAddStrength = .75;

    fractalField.displacements = vec3(0);
    // fractalField.displacements.y =
    //     250 * texture(audioR, 2. * abs(resolution.x / 2. - particle.position.x) / resolution.x).x;

    fractalField.gamma = 2;
    fractalField.flows = vec4(-12, 0, 0, 2.);
    fractalField.fScale = 1.;
    fractalField.octaveScale = 2.8;
    // fractalField.octaveMultiplier = .5;
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
    glow.intensity = .2;
    glow.size = vec2(17);
    glow.directions = 16.0;
    glow.quality = 6.0;
    glow.color = vec4(0.5412, 0.0, 0.4235, 0.303);
    glow.brightnessOffset = .5;
    glow.lightStrength = .92;
}
