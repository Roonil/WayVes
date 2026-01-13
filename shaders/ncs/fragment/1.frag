#version 430 core

precision highp float;

uniform vec2 resolution;

uniform sampler1D audioL;
uniform sampler1D audioR;

uniform float time;
uniform int audioLSize;
uniform int audioRSize;

layout(r32ui, binding = 0) uniform uimage2D atomicImageTexture0;
layout(r32ui, binding = 1) uniform uimage2D atomicImageTexture1;
layout(r32ui, binding = 2) uniform uimage2D atomicImageTexture2;
layout(r32ui, binding = 3) uniform uimage2D atomicImageTexture3;
layout(r32ui, binding = 4) uniform uimage2D atomicImageTexture4;

out vec4 FragColor;

#include ":utils/lygia/generative/pnoise.glsl"

#include ":$CONFIGFILE"

void defaultAudioValues()
{
    audio.value = 0;
    audio.bass = 0;
    audio.mixing = 0.5;
    audio.multiplier = 7;
    audio.bassMultiplier = 5;

    audio.exponentiationFactor = 1.02;

    audio.samplePoints[0] = 0.1;
    audio.samplePoints[1] = 0.2;
    audio.samplePoints[2] = 0.3;
    audio.samplePoints[3] = 0.4;
    audio.samplePoints[4] = 0.5;
    audio.samplePoints[5] = 0.6;
    audio.samplePoints[6] = 0.7;
    audio.samplePoints[7] = 0.8;
    audio.samplePoints[8] = 0.9;

    audio.samplePointsDifferences[0] = 0.05;
    audio.samplePointsDifferences[1] = 0.05;
    audio.samplePointsDifferences[2] = 0.05;
    audio.samplePointsDifferences[3] = 0.05;
    audio.samplePointsDifferences[4] = 0.05;
    audio.samplePointsDifferences[5] = 0.05;
    audio.samplePointsDifferences[6] = 0.05;
    audio.samplePointsDifferences[7] = 0.05;
    audio.samplePointsDifferences[8] = 0.05;

    audio.intermediateAudios[0] = 0;
    audio.intermediateAudios[1] = 0;
    audio.intermediateAudios[2] = 0;
    audio.intermediateAudios[3] = 0;
    audio.intermediateAudios[4] = 0;
    audio.intermediateAudios[5] = 0;
    audio.intermediateAudios[6] = 0;
    audio.intermediateAudios[7] = 0;
}

void defaultBaseFormValues()
{
    baseForm.type = 0;
    baseForm.scale = vec3(2.0);
    baseForm.numParticles = vec3(resolution.xy, 1);
    baseForm.zSize = 100;
    baseForm.rotations = IDENTITY_MATRIX;
    baseForm.rotationCenter = vec3(resolution.xy / 2, 0);
}

void defaultParticleValues()
{
    particle.color = vec4(0, 0, 1, 1);
    particle.size = 3;
    particle.feather = 0.5;
    particle.position = vec3(gl_FragCoord.xy, 0);
    particle.opacityMultiplier = 1;
    particle.colorIntensityAddStrength = 0.1;
    particle.antiAlias = 4.5;
}

void defaultFractalFieldValues()
{
    fractalField.octaveMultiplier = 0.5;
    fractalField.octaveScale = 1.5;
    fractalField.complexity = 3;
    fractalField.fScale = 10;
    fractalField.gamma = 1;
    fractalField.minVal = -1;
    fractalField.maxVal = 1;

    fractalField.noise = vec3(0);

    fractalField.affectOpacity = 0;
    fractalField.affectSize = 0;

    fractalField.loop = 0;
    fractalField.loopFrames = 200;

    fractalField.dimensions = vec4(1000);

    fractalField.displacementType = 0;

    fractalField.offset = 0;
    fractalField.noiseMultiplier = 1;
    fractalField.constantNoiseMultiplier = 0;

    fractalField.displacements = vec3(100);
    fractalField.flows = vec4(0, 0, 0, 2.);
}

void defaultSphereValues()
{
    sphere.radius = 0;
    sphere.feather = 0;
    sphere.scale = vec3(1);
    sphere.strength = 1;
    sphere.center = vec3(resolution.xy / 2, 0);
}

void setAudio()
{

    float audioRadius = (max(texture(audioR, audio.samplePoints[0]).x, texture(audioR, audio.samplePoints[0] + audio.samplePointsDifferences[0]).x) + max(texture(audioL, audio.samplePoints[0]).x, texture(audioL, audio.samplePoints[0] + audio.samplePointsDifferences[0]).x)) / 2;
    float audioFractal1 = (max(texture(audioR, audio.samplePoints[1]).x, texture(audioR, audio.samplePoints[1] + audio.samplePointsDifferences[1]).x) + max(texture(audioL, audio.samplePoints[1]).x, texture(audioL, audio.samplePoints[1] + audio.samplePointsDifferences[1]).x)) / 2;
    float audioFractal2 = (max(texture(audioR, audio.samplePoints[2]).x, texture(audioR, audio.samplePoints[2] + audio.samplePointsDifferences[2]).x) + max(texture(audioL, audio.samplePoints[2]).x, texture(audioL, audio.samplePoints[2] + audio.samplePointsDifferences[2]).x)) / 2;
    float audioFractal3 = (max(texture(audioR, audio.samplePoints[3]).x, texture(audioR, audio.samplePoints[3] + audio.samplePointsDifferences[3]).x) + max(texture(audioL, audio.samplePoints[3]).x, texture(audioL, audio.samplePoints[3] + audio.samplePointsDifferences[3]).x)) / 2;
    float audioFractal4 = (max(texture(audioR, audio.samplePoints[4]).x, texture(audioR, audio.samplePoints[4] + audio.samplePointsDifferences[4]).x) + max(texture(audioL, audio.samplePoints[4]).x, texture(audioL, audio.samplePoints[4] + audio.samplePointsDifferences[4]).x)) / 2;
    float audioFractal5 = (max(texture(audioR, audio.samplePoints[5]).x, texture(audioR, audio.samplePoints[5] + audio.samplePointsDifferences[5]).x) + max(texture(audioL, audio.samplePoints[5]).x, texture(audioL, audio.samplePoints[5] + audio.samplePointsDifferences[5]).x)) / 2;
    float audioFractal6 = (max(texture(audioR, audio.samplePoints[6]).x, texture(audioR, audio.samplePoints[6] + audio.samplePointsDifferences[6]).x) + max(texture(audioL, audio.samplePoints[6]).x, texture(audioL, audio.samplePoints[6] + audio.samplePointsDifferences[6]).x)) / 2;
    float audioFractal7 = (max(texture(audioR, audio.samplePoints[7]).x, texture(audioR, audio.samplePoints[7] + audio.samplePointsDifferences[7]).x) + max(texture(audioL, audio.samplePoints[7]).x, texture(audioL, audio.samplePoints[7] + audio.samplePointsDifferences[7]).x)) / 2;
    float audioFractal8 = (max(texture(audioR, audio.samplePoints[8]).x, texture(audioR, audio.samplePoints[8] + audio.samplePointsDifferences[8]).x) + max(texture(audioL, audio.samplePoints[8]).x, texture(audioL, audio.samplePoints[8] + audio.samplePointsDifferences[8]).x)) / 2;

    float audios[8] = { audioFractal1, audioFractal2, audioFractal3, audioFractal4, audioFractal5, audioFractal6, audioFractal7, audioFractal8 };

    float temp;
    temp = max(audios[0], audios[2]);
    audios[0] = min(audios[0], audios[2]);
    audios[2] = temp;

    temp = max(audios[1], audios[3]);
    audios[1] = min(audios[1], audios[3]);
    audios[3] = temp;

    temp = max(audios[4], audios[6]);
    audios[4] = min(audios[4], audios[6]);
    audios[6] = temp;

    temp = max(audios[5], audios[7]);
    audios[5] = min(audios[5], audios[7]);
    audios[7] = temp;

    temp = max(audios[0], audios[4]);
    audios[0] = min(audios[0], audios[4]);
    audios[4] = temp;

    temp = max(audios[1], audios[5]);
    audios[1] = min(audios[1], audios[5]);
    audios[5] = temp;

    temp = max(audios[2], audios[6]);
    audios[2] = min(audios[2], audios[6]);
    audios[6] = temp;

    temp = max(audios[3], audios[7]);
    audios[3] = min(audios[3], audios[7]);
    audios[7] = temp;

    temp = max(audios[0], audios[1]);
    audios[0] = min(audios[0], audios[1]);
    audios[1] = temp;

    temp = max(audios[2], audios[3]);
    audios[2] = min(audios[2], audios[3]);
    audios[3] = temp;

    temp = max(audios[4], audios[5]);
    audios[4] = min(audios[4], audios[5]);
    audios[5] = temp;

    temp = max(audios[6], audios[7]);
    audios[6] = min(audios[6], audios[7]);
    audios[7] = temp;

    temp = max(audios[2], audios[4]);
    audios[2] = min(audios[2], audios[4]);
    audios[4] = temp;

    temp = max(audios[3], audios[5]);
    audios[3] = min(audios[3], audios[5]);
    audios[5] = temp;

    temp = max(audios[1], audios[4]);
    audios[1] = min(audios[1], audios[4]);
    audios[4] = temp;

    temp = max(audios[3], audios[6]);
    audios[3] = min(audios[3], audios[6]);
    audios[6] = temp;

    temp = max(audios[1], audios[2]);
    audios[1] = min(audios[1], audios[2]);
    audios[2] = temp;

    temp = max(audios[3], audios[4]);
    audios[3] = min(audios[3], audios[4]);
    audios[4] = temp;

    temp = max(audios[5], audios[6]);
    audios[5] = min(audios[5], audios[6]);
    audios[6] = temp;

    audio.value = audio.multiplier * mix(mix(audios[7] * audios[6] - audios[1] * audios[0], audios[7] * audios[6], audios[5]), mix(audios[6] * mix(audios[7] - audios[0], audios[6] - audios[3], audios[7] * audios[6]) - pow(audios[1] * audios[0], audio.exponentiationFactor), audios[7] * audios[6], audios[5] * audios[4]), audio.mixing);

    audio.bass = audio.bassMultiplier * abs(audioRadius);

    audio.intermediateAudios[0] = audioFractal1;
    audio.intermediateAudios[1] = audioFractal2;
    audio.intermediateAudios[2] = audioFractal3;
    audio.intermediateAudios[3] = audioFractal4;
    audio.intermediateAudios[4] = audioFractal5;
    audio.intermediateAudios[5] = audioFractal6;
    audio.intermediateAudios[6] = audioFractal7;
    audio.intermediateAudios[7] = audioFractal8;
}

float octaveNoise(vec4 p, vec4 flow, vec4 rep)
{

    float total = 0.0;
    float frequency = 1.0;
    float amplitude = 1.0;
    float value = 0.0;

    for (int i = 0; i < fractalField.complexity; i += 1) {

        vec4 fractalFieldInput = p;
        modifyNoiseCoordinates(fractalFieldInput);

        fractalFieldInput += flow * time;

        fractalFieldInput *= frequency;

        value += (pnoise(vec4((fractalFieldInput)), rep)) * amplitude;
        total += amplitude;

        amplitude *= fractalField.octaveMultiplier;
        frequency *= fractalField.octaveScale;
    }
    return value / total;
}

float fbm3(vec4 p, vec4 flow)
{
    vec4 flowXLoopFrames = flow * fractalField.loopFrames;

    vec4 rep = fractalField.loop * ivec4(fractalField.fScale * flowXLoopFrames / fractalField.dimensions);

    flowXLoopFrames = mix(vec4(1), flowXLoopFrames, 1. - step(abs(flowXLoopFrames), vec4(0)));

    vec4 newFScale = mix(fractalField.dimensions * rep / (flowXLoopFrames), vec4(fractalField.fScale), vec4(1) - abs(fractalField.loop * sign(flow)));

    p = newFScale * p / fractalField.dimensions;
    flow *= newFScale / fractalField.dimensions;

    vec3 originalSphereCenter = sphere.center;
    sphere.center *= newFScale.xyz / fractalField.dimensions.xyz;

    float oN = (fractalField.constantNoiseMultiplier + audio.value) * (octaveNoise(p, flow, rep));

    oN = sign(oN) * pow(abs(oN), fractalField.gamma);

    sphere.center = originalSphereCenter;

    oN = fractalField.offset + fractalField.noiseMultiplier * oN;

    oN = clamp(oN, fractalField.minVal, fractalField.maxVal);

    return oN;
}

vec3 sphereCoords(vec3 particleCoords, float zLayer, float zLayerDistance)
{

    vec3 newPos;

    float u = (TWOPI * (((particleCoords.x) / (resolution.x))));
    float v = PI * (particleCoords.y / resolution.y);

    newPos.x = resolution.x * sin(u) * sin(v);
    newPos.z = baseForm.zSize * cos(u) * sin(v);
    newPos.y = resolution.y * cos(v);

    newPos.xy += resolution.xy / 2.;

    newPos -= zLayer * (vec3(resolution.xy / 2., baseForm.zSize / 2.) / baseForm.numParticles.z) * normalize(newPos - vec3(resolution.xy / 2, 0));

    return newPos;
}

vec3 transformedCoords(vec3 particleCoords)
{

    particleCoords = (baseForm.rotations) * (particleCoords - baseForm.rotationCenter);

    return (particleCoords + vec3(resolution.xy / 2, 0));
}

void processZLayer(int zLayer, float zLayerDistance)
{

    vec3 particleCoords = particle.position;

    particleCoords = mix(particleCoords, sphereCoords(particleCoords, zLayer, zLayerDistance), baseForm.type);

    vec4 old = vec4(particleCoords, 0);
    vec3 displacementValues = vec3(0);

    vec4 flows = fractalField.flows;

    float xFBM3 = fbm3(old.xyzw, flows);
    float yFBM3 = fbm3(old.yzxw, flows.yzxw);
    float zFBM3 = fbm3(old.zxyw, flows.zxyw);

    fractalField.noise = vec3(xFBM3, yFBM3, zFBM3);

    setPropsWithNoise();

    displacementValues.xyz += mix(vec3((fractalField.displacements.x) * xFBM3, (fractalField.displacements.y) * yFBM3, (fractalField.displacements.z) * zFBM3), (fractalField.displacements.x) * xFBM3 * normalize(particleCoords.xyz - vec3(resolution.xy / 2, 0)), fractalField.displacementType);

    particleCoords.xyz += displacementValues;

    float radius = sphere.radius, blurSize = particle.antiAlias / resolution.y;
    radius += audio.bass;

    vec3 sphereCenterCoords = sphere.center;
    vec3 vectorFromSphereCenter = (particleCoords - sphereCenterCoords);

    vec3 normalizedVector = normalize(vectorFromSphereCenter);
    vec3 newPos = (sphereCenterCoords + radius * normalizedVector);

    float diff = length(newPos - particleCoords);
    diff *= (clamp((smoothstep(0.0, sphere.feather * (radius), diff)) + blurSize, blurSize, 1.0 + blurSize));

    particleCoords += step(length(vectorFromSphereCenter), radius) * sphere.strength * diff * normalizedVector * sphere.scale;

    modifySphericalDisplacement();

    particle.size = int(max(0., particle.size + fractalField.affectSize * (xFBM3 + yFBM3 + zFBM3)));
    particle.opacityMultiplier = max(particle.opacityMultiplier + fractalField.affectOpacity * (xFBM3 + yFBM3 + zFBM3), 0.);

    for (int i = -particle.size; i <= particle.size; i++)
        for (int j = -particle.size; j <= particle.size; j++) {
            float distanceFromParticleCenter = length(vec2(i, j));
            distanceFromParticleCenter = mix(step(distanceFromParticleCenter, float(particle.size)), (1. - smoothstep(particle.size - particle.feather * particle.size, (particle.size), distanceFromParticleCenter)), particle.feather);
            distanceFromParticleCenter *= 100000. * particle.opacityMultiplier;

            vec3 finalCoords = vec3(transformedCoords(particleCoords.xyz) + vec3(i, j, 0)) / baseForm.scale;
            finalCoords += vec3(resolution.xy / 2., 0) * (1. - 1. / (baseForm.scale));

            uint depth = imageAtomicAdd(atomicImageTexture0, ivec2(finalCoords.xy), uint((distanceFromParticleCenter)));

            if ((colorTracking) == 1.) {
                uint colorValue = imageAtomicAdd(atomicImageTexture1, ivec2(finalCoords.xy), uint(clamp(particle.color.x * distanceFromParticleCenter, 0., 100000.)));
                colorValue = imageAtomicAdd(atomicImageTexture2, ivec2(finalCoords.xy), uint(clamp(particle.color.y * distanceFromParticleCenter, 0., 100000.)));
                colorValue = imageAtomicAdd(atomicImageTexture3, ivec2(finalCoords.xy), uint(clamp(particle.color.z * distanceFromParticleCenter, 0., 100000.)));
                colorValue = imageAtomicAdd(atomicImageTexture4, ivec2(finalCoords.xy), uint(clamp(particle.color.w * distanceFromParticleCenter, 0., 100000.)));
            }
        }
}

void main()
{
    defaultAudioValues();
    defaultBaseFormValues();
    defaultParticleValues();
    defaultFractalFieldValues();
    defaultSphereValues();

    init();
    setAudio();

    ivec3 spaces = ivec3(round((vec3(resolution.xy, baseForm.zSize)) / (vec3(baseForm.numParticles - step(baseForm.type, 0) * vec3(1, 1, 1)))));

    spaces -= int(step(baseForm.type, 0)) * ivec3(1, 1, 1);

    particle.position.z -= (baseForm.numParticles.z - 1) * (spaces.z) / 2.;

    particle.position.xy += step(baseForm.numParticles.xy, vec2(1)) * vec2(resolution.xy / 2.);

    int currentZIndex = 0;
    for (float i = 0; i < baseForm.zSize; i += spaces.z) {

        if ((length(ivec3(mod(vec3(gl_FragCoord.xy, 0), spaces))) > 0))
            discard;

        setProps();

        processZLayer(currentZIndex, (baseForm.numParticles.z - 1) * (spaces.z) / 2.);
        currentZIndex += 1;

        baseForm.rotations = IDENTITY_MATRIX;
        sphere.center = vec3(resolution.xy / 2., 0);
        particle.position.xy = gl_FragCoord.xy;
        particle.position.z = i + spaces.z - (baseForm.numParticles.z - 1) * (spaces.z) / 2.;

        particle.position.xy += step(baseForm.numParticles.xy, vec2(1)) * vec2(resolution.xy / 2.);
    }
}
