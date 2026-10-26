#include ":ncs/structs.glsl"

#include ":matugenColors/colors.glsl"
#include ":utils/post-processing/glow/structs.glsl"

#define colorTracking 1

const mat3 RGB_TO_YIQ = mat3(0.299, 0.596, 0.211, 0.587, -0.274, -0.523, 0.114, -0.322, 0.312);
const mat3 YIQ_TO_RGB = mat3(1.0, 1.0, 1.0, 0.956, -0.272, -1.106, 0.621, -0.647, 1.703);

const vec3 paletteAnchor = vec3(0.1176, 0.0, 1.0);

float themeHueShift()
{
    vec3 theme = RGB_TO_YIQ * matugenColors.primary.xyz;
    vec3 anchor = RGB_TO_YIQ * paletteAnchor;

    float shift = atan(theme.z, theme.y) - atan(anchor.z, anchor.y);
    shift = mod(shift + PI, 2. * PI) - PI;

    return shift * smoothstep(0.02, 0.06, length(theme.yz));
}

vec3 rotateHue(vec3 rgb, float angle)
{
    vec3 yiq = RGB_TO_YIQ * rgb;
    float c = cos(angle);
    float s = sin(angle);
    yiq.yz = mat2(c, s, -s, c) * yiq.yz;
    return clamp(YIQ_TO_RGB * yiq, 0., 1.);
}

void init()
{
    audio.multiplier = 0;
    baseForm.numParticles = vec3(1500, 1200, 1);
    baseForm.zSize = resolution.x;
    baseForm.scale = vec3(1., 1.5, 1);
    baseForm.zSize = resolution.y;
    // baseForm.scale.x = 1;
    // baseForm.scale.y = .85;
}

float getMixVal(float startValue, float endValue)
{
    return smoothstep(startValue * resolution.x, endValue * resolution.x, particle.position.x);
}

void setColors()
{
    particle.color = mix(vec4(0.3255, 1.0, 0.349, 0.386), vec4(0.3765, 1.0, 0.8863, 0.351), getMixVal(0, .3));
    particle.color =
        mix(particle.color, vec4(0.3059, 0.6275, 1.0, 0.42),
            getMixVal(.2 + clamp(texture(audioR, .1).x * 3, 0., .2), .4 + clamp(texture(audioR, .1).x * 3, 0., .2)));
    particle.color = mix(particle.color, vec4(0.1176, 0.0, 1.0, 0.448), getMixVal(.4, .6));
    particle.color = mix(particle.color, vec4(0.8039, 0.2706, 1.0, 0.478),
                         getMixVal(.6, .8 + clamp(texture(audioR, .1).x * 3, 0., .2)));
    particle.color = mix(particle.color, vec4(0.7333, 0.0, 1.0, 0.478), getMixVal(.8, 1));

    particle.color.xyz = rotateHue(particle.color.xyz, themeHueShift());
}

void setProps()
{

    setColors();
    // particle.color = interpolateHue(vec4(0.251, 0.6235, 0.5255, 1.0), .3, particle.position.x, resolution.x);
    // particle.color = mix(particle.color, vec4(1), abs(particle.position.z) / (2 * baseForm.zSize));
    particle.size = 1;
    particle.feather = 0.5;

    baseForm.rotations = rotateX(90);

    particle.feather += smoothstep(0., resolution.y / 2., abs(particle.position.y - resolution.y / 2.));

    fractalField.loop = 1;
    fractalField.loopFrames = 800;
    // baseForm.rotations = rotateX(time * smoothstep(0., 1., particle.position.x / resolution.x) * .1);
    particle.colorIntensityAddStrength = .4;

    particle.color.xyz = mix(particle.color.xyz, vec3(.5),
                             clamp(1.2 * texture(audioR, .05 * particle.position.x / resolution.x).x, 0., 1.));
    particle.colorIntensityAddStrength += 1 * abs(texture(audioR, particle.position.x / resolution.x).x);
    // particle.colorIntensityAddStrength = clamp(particle.colorIntensityAddStrength, 0., 1.);
    fractalField.constantNoiseMultiplier = .5;

    fractalField.displacements = vec3(500, 900, 1000);

    fractalField.affectOpacity = 2 + 3 * abs(texture(audioR, particle.position.x / resolution.x).x);

    fractalField.flows = vec4(-.75, 0, 0, 0.75);
    fractalField.fScale = 4;
    fractalField.complexity = 5;

    fractalField.octaveScale = 1.5;
    fractalField.octaveMultiplier = .5;
}

void modifyNoiseCoordinates(inout vec4 coords)
{
}

void setPropsWithNoise()
{
}

void modifySphericalDisplacement()
{
    particle.feather += smoothstep(0., baseForm.zSize / 2., abs(particle.position.z - baseForm.zSize / 2.));
}

void setGlow0(inout Glow glow)
{
    glow.blendMode = 0;
    glow.mixAlpha = 0;

    glow.size = vec2(7);
    glow.directions = 16.0;
    glow.quality = 6.0;
    glow.color = vec4(rotateHue(vec3(0.5412, 0.0, 0.4235), themeHueShift()), 1);
    glow.brightnessOffset = .5;
    glow.lightStrength = .92;
}
