#include "linear/structs.glsl"
#include "utils/post-processing/glow/structs.glsl"

#define coordinateRotation 0.

#define horizontalDistanceFromEdges 10

#define fragmentWidth 1
#define leftPadding (horizontalDistanceFromEdges)
#define rightPadding (horizontalDistanceFromEdges)

#define visualiserDirections 2
#define visualiserMode 2

float reflectionLineHeight = 2;
float bottomReflectionSize = 10;
float softnessModifier = 400;
float taperSmoothingDistance = 100;

float maxWaveHeight = resolution.y - (reflectionLineHeight + bottomReflectionSize);

void init()
{

    particle.connector.left.enable = 1;
    particle.connector.right.enable = 1;
}

void audioFetch(inout float fetchedAudio, float n, float lastN)
{
}

void setOffsets(float direction, inout vec2 particleOffset, inout vec2 barOffset, inout vec2 barSizeOffset,
                vec2 barAudio, vec2 particleAudio, float xCoordinate, float n, float lastN)
{

    float smoothedAudioValue = smoothstep(0.25, 1., texture(audioR, .1).x);

    float amplitude = (maxWaveHeight / 2. - maxWaveHeight / 16.);

    float normalisedXCoord = (r_gl_FragCoord.x / (r_resolution.x - rightPadding - leftPadding));

    float topConnectorsOffset =
        amplitude *
        sin((.03) * (normalisedXCoord * 500 * (1 + .5 * smoothedAudioValue) + 1000 * smoothedAudioValue + time));
    float bottomConnectorsOffset = (amplitude - maxWaveHeight / 10.) *
                                   sin((.03) * (normalisedXCoord * 400 * (1 + .5 * smoothedAudioValue) +
                                                400 * smoothedAudioValue + smoothedAudioValue - 12 + 2 * time));

    normalisedXCoord = normalisedXCoord * 2 - 1;

    float beta = 5;
    float taperStart = .2;
    float ax = abs(normalisedXCoord);

    float t = clamp((ax - taperStart) / (1. - taperStart), 0., 1.);

    float envelope = exp(-beta * t * t);

    topConnectorsOffset *= envelope;
    bottomConnectorsOffset *= envelope;

    barSizeOffset.x = topConnectorsOffset - bottomConnectorsOffset;
    barOffset.y = bottomConnectorsOffset;

    particleOffset.y = mix(topConnectorsOffset, bottomConnectorsOffset, direction);
}

void setProps()
{

    bar.size = vec3(0, 1, 0);
    bar.borderSize = vec3(1, 0, 1);

    bar.color = vec4(0.0, 0.298, 1.0, 1.0);
    bar.borderColor = vec4(0.1294, 0.0, 0.298, 0.345);

    bar.innerSoftness = vec3(softnessModifier / 6.25, 0, softnessModifier / 6.25);
    bar.outerSoftness = vec3(softnessModifier / 33., 0, softnessModifier / 33.);

    particle.connector.jointMode = 1;

    particle.connector.left.color = vec4(1);

    particle.connector.left.height = 6;
    particle.connector.left.borderSize = 2;

    particle.connector.left.innerSoftness = 3;
    particle.connector.left.outerSoftness = 8;

    particle.connector.left.color =
        mix(vec4(0.0, 1.0, 0.949, 1.0),
            mix(vec4(1), vec4(0.9686, 0.0, 1.0, 1.0), 2. * particle.fragment.coords.x / resolution.x),
            particle.fragment.coords.x / resolution.x);

    // particle.connector.left.borderColor = vec4(0.1294, 0.0, 0.298, 1.0);

    float leftSideTaper =
        smoothstep(-(horizontalDistanceFromEdges), taperSmoothingDistance - (horizontalDistanceFromEdges),
                   particle.fragment.coords.x);
    float rightSideTaper = 1. - smoothstep(resolution.x - taperSmoothingDistance - (horizontalDistanceFromEdges),
                                           resolution.x - (horizontalDistanceFromEdges), particle.fragment.coords.x);

    particle.connector.left.height *= leftSideTaper;
    particle.connector.left.height *= rightSideTaper;

    bar.borderSize.xz *= leftSideTaper * rightSideTaper;
    bar.size.xz *= leftSideTaper * rightSideTaper;

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
    particle.fragment.coords = glow.coords;
    setProps();

    glow.intensity = 2.5;
    glow.color = particle.connector.left.color *
                 mix(vec4(0.0, 0.0, 0.0, 1.0), vec4(1), clamp(3 * glow.coords.x / resolution.x, 0., 1.));

    glow.directions = 1;
    glow.blendMode = 1;
    glow.mixAlpha = 1;

    glow.size = vec2(12);
    glow.quality = 12;
    glow.brightnessOffset = .8;
    glow.lightStrength = .5;
}

void setGlow1(inout Glow glow)
{
    particle.fragment.coords = glow.coords;
    setProps();

    float originalCoords = glow.coords.y;

    float midPoint = (resolution.y / 2. - (maxWaveHeight / 2. - maxWaveHeight / 16. + particle.connector.left.height));
    glow.coords.y = 2 * midPoint - glow.coords.y;

    glow.color = particle.connector.left.color;

    float reflectionStep = step(midPoint - reflectionLineHeight, originalCoords);

    glow.intensity = mix(1., 12., reflectionStep);
    glow.color.w *= mix((smoothstep(midPoint - reflectionLineHeight - bottomReflectionSize, midPoint, originalCoords)),
                        smoothstep(midPoint - reflectionLineHeight, midPoint, originalCoords), reflectionStep);

    glow.size = vec2(mix(12., 70., reflectionStep), 2);

    glow.directions = 16;
    glow.blendMode = 1;

    glow.mixAlpha = 1;

    glow.quality = 12;
}