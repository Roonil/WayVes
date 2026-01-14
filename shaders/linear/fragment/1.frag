#version 430 core

in vec2 r_resolution;
in vec4 r_ngl_FragCoord;

uniform vec2 resolution;

uniform sampler1D audioL;
uniform sampler1D audioR;

uniform float time;
uniform int audioLSize;
uniform int audioRSize;

layout(binding = 0, rgba32f) uniform restrict image2D imageTexture0;
layout(binding = 1, rgba32f) uniform restrict image2D imageTexture1;

out vec4 FragColor;

vec4 r_gl_FragCoord = vec4(floor(r_ngl_FragCoord.xy * r_resolution.xy) + .5, r_ngl_FragCoord.zw);

#define TWOPI 6.28318530718
#define PI 3.14159265359

#include ":$CONFIGFILE"

float cutOff = 0;

vec4 topParticleColor = vec4(0), topParticleBorderColor = vec4(0),
     topParticleCapColor = vec4(0),
     topParticleConnectorColor = vec4(0), topParticleConnectorBorderColor = vec4(0),
     topParticleRightConnectorColor = vec4(0), topParticleRightConnectorBorderColor = vec4(0);

#define getNthCenter(n) vec2(((2. * (n) + 1.) * ((fragmentWidth) / 2.)), r_resolution.y / 2.)

float applySoftness(float softness, float startVal, float endVal)
{
    return mix(step(endVal, startVal), 1. - smoothstep(startVal - softness, startVal, endVal), sign(softness));
}

float halfConnectorSDF(vec2 pos, vec2 firstCenter, vec2 secondCenter)
{
    vec2 v1 = secondCenter - firstCenter;
    vec2 v2 = pos - firstCenter;
    float d = clamp(dot(v2, v1) / dot(v1, v1), 0.0, 1.0);
    return length(v2 - d * v1);
}

vec4 addColors(vec4 above, vec4 below)
{

    return above + (1 - above.w) * below;
}

int connectorSDF(int particleSelector)
{

    vec2 pos = particle.fragment.coords;

    Connector connector = particle.connector;

    float outerVal = halfConnectorSDF(pos, particle.connector.currentCenter, particle.connector.rightCenter);

    float d1 = outerVal;
    float W = connector.right.height / 2. + connector.right.borderSize;

    outerVal -= W;
    float oAA = connector.right.outerSoftness;

    float rightHalfConnector = (applySoftness(oAA, W, (outerVal)));

    outerVal = halfConnectorSDF(pos, particle.connector.leftCenter, particle.connector.currentCenter);
    float d2 = outerVal;

    W = (connector.left.height / 2. + connector.left.borderSize);
    outerVal -= W;
    oAA = connector.left.outerSoftness;

    float leftHalfConnector = (applySoftness(oAA, W, (outerVal)));

    float isLast = mix(0, 1. - step(particle.fragment.lastN, particle.fragment.n), particle.connector.right.enable);
    float isFirst = mix(0, 1 - step(particle.fragment.n, 0), particle.connector.left.enable);

    vec4 finalVal = vec4(0);
    finalVal.w = isLast * rightHalfConnector;
    finalVal.y = isFirst * leftHalfConnector;

    int leftInnerSelector = int(mix(PARTICLE_UP_LEFT_CONNECTOR_INNER_SDF, PARTICLE_DOWN_LEFT_CONNECTOR_INNER_SDF, particleSelector));
    int rightInnerSelector = int(mix(PARTICLE_UP_RIGHT_CONNECTOR_INNER_SDF, PARTICLE_DOWN_RIGHT_CONNECTOR_INNER_SDF, particleSelector));
    int leftOuterSelector = int(mix(PARTICLE_UP_LEFT_CONNECTOR_OUTER_SDF, PARTICLE_DOWN_LEFT_CONNECTOR_OUTER_SDF, particleSelector));
    int rightOuterSelector = int(mix(PARTICLE_UP_RIGHT_CONNECTOR_OUTER_SDF, PARTICLE_DOWN_RIGHT_CONNECTOR_OUTER_SDF, particleSelector));

    sdfs[leftOuterSelector] = cutOff * finalVal.y;
    sdfs[rightOuterSelector] = cutOff * finalVal.w;

    float val = isLast * rightHalfConnector + isFirst * leftHalfConnector;
    val -= isFirst * isLast * rightHalfConnector * leftHalfConnector;
    W = connector.right.height / 2.;
    float innerVal = d1 - W;
    float iAA = connector.right.innerSoftness;

    rightHalfConnector = (applySoftness(iAA, W, (innerVal)));

    W = (connector.left.height / 2.);
    iAA = connector.left.innerSoftness;
    innerVal = d2 - W;

    leftHalfConnector = (applySoftness(iAA, W, (innerVal)));

    float val2 = isLast * rightHalfConnector + isFirst * leftHalfConnector;

    finalVal.z = isLast * rightHalfConnector;
    finalVal.x = isFirst * leftHalfConnector;

    sdfs[leftInnerSelector] = cutOff * finalVal.x;
    sdfs[rightInnerSelector] = cutOff * finalVal.z;

    float t1 = sdfs[leftOuterSelector], t2 = sdfs[rightOuterSelector];
    float t3 = sdfs[leftInnerSelector];
    float t4 = sdfs[rightInnerSelector];

    float jM = particle.connector.jointMode;
    float sJM0 = step(jM, 0.), sJM1 = step(jM, 1.);
    float jM0 = sJM0, jM1 = (1 - sJM0) * sJM1;

    float innerSJM2Connector = clamp((t4 + t3 - t3 * t4), 0, 1);
    float outerSJM2Connector = clamp((t1 + t2 - t1 * t2), 0, 1);

    float innerSJM1Connector = max(t3, t4);
    float outerSJM1Connector = max(t1, t2);

    float innerRightJointSelector = mix(1. - sign(t3), sign(t4), particle.connector.jointColorMode);
    float outerRightJointSelector = mix(sign(t2), 1. - sign(t1), particle.connector.jointColorMode);

    float innerRightJMSelector = innerRightJointSelector * mix(innerSJM2Connector, innerSJM1Connector, jM1);
    float innerLeftJMSelector = (1. - innerRightJointSelector) * mix(innerSJM2Connector, innerSJM1Connector, jM1);
    float outerRightJMSelector = mix(max(outerRightJointSelector * outerSJM2Connector - innerSJM2Connector, 0), max(outerRightJointSelector * outerSJM1Connector - innerSJM1Connector, 0), jM1);
    float outerLeftJMSelector = mix(max((1. - outerRightJointSelector) * outerSJM2Connector - innerSJM2Connector, 0), max((1. - outerRightJointSelector) * outerSJM1Connector - innerSJM1Connector, 0), jM1);

    sdfs[rightInnerSelector] = mix(innerRightJMSelector, sdfs[rightInnerSelector], jM0);
    sdfs[leftInnerSelector] = mix(innerLeftJMSelector, sdfs[leftInnerSelector], jM0);

    sdfs[rightOuterSelector] = mix(outerRightJMSelector, sdfs[rightOuterSelector] - sdfs[rightInnerSelector], jM0);
    sdfs[leftOuterSelector] = mix(outerLeftJMSelector, sdfs[leftOuterSelector] - sdfs[leftInnerSelector], jM0);

    return 0;
}

float circleSDF(float d, float iAA, float r)
{

    return (applySoftness(iAA, r, d));
}

float capSDF(float mode, float direction, float barSpan, float audioVal, float yOffset, Cap cap)
{
    float directionSelector = (2 * direction - 1);

    ivec2 imageCoords = ivec2(gl_FragCoord.xy);

    vec4 img0Data = imageLoad(imageTexture0, imageCoords);
    vec4 img1Data = imageLoad(imageTexture1, imageCoords);

    vec4 capData = mix(img0Data, img1Data, mode * (1 - step((visualiserMode), 1)));

    float oldCapVal = mix(capData.x, capData.z, direction);
    float capVal = directionSelector * -audioVal;

    float dR = mix(capData.y, capData.w, direction);
    float eDR = dR * cap.elasticity;

    float valSelector = mix(oldCapVal - capVal, capVal - oldCapVal, direction);

    float accelSelector = 1 - step(valSelector, dR);

    dR += accelSelector * cap.acceleration;

    dR = mix(dR, -eDR * (1. - step(abs(dR), cap.elasticityMinThreshold)), (1. - accelSelector) * (1. - step(eDR, valSelector)));
    dR = mix(dR, min(0, dR + cap.rate), 1 - step(0, dR));
    dR = mix(dR, -(cap.launchVelocity + cap.launchFlingMultiplier * mix(capVal - oldCapVal, oldCapVal - capVal, direction)), (1 - mix(step(capVal, oldCapVal + cap.rate), step(oldCapVal, capVal + cap.rate), direction)));

    dR *= 1. - cap.dragFactor;

    capVal = mix(max(capVal, oldCapVal - (dR) - (1. - cap.dragFactor) * cap.rate),
        min(capVal, oldCapVal + (dR) + (1. - cap.dragFactor) * cap.rate),
        direction);

    vec2 newData = vec2(capVal, dR);

    imageStore(imageTexture0, imageCoords,

        mix(mix(vec4(newData, img0Data.zw), vec4(img0Data.xy, newData), direction),
            img0Data,
            mode * (1 - step((visualiserMode), 1)))

    );

    imageStore(imageTexture1, imageCoords,

        mix(mix(vec4(newData, img1Data.zw), vec4(img1Data.xy, newData), direction),
            img1Data,
            1. - mode * (1 - step((visualiserMode), 1)))

    );

    float width = applySoftness(cap.softness.y, cap.size.x / 2., abs(barSpan + cap.offset.x));
    float bH = mix((capVal + cap.offset.y), (capVal - cap.size.y - 2 * yOffset), direction) + cap.size.y / 2. + yOffset + r_resolution.y / 2. - r_gl_FragCoord.y;
    float height = applySoftness(cap.softness.z, cap.size.y / 2., bH);

    height *= applySoftness(cap.softness.x, cap.size.y / 2., -bH);

    return width * height;
}

int particleSDF(int particleSelector)
{

    vec2 currentCenter = particle.connector.currentCenter;
    vec2 pos = particle.fragment.coords;

    float dist = length(pos - currentCenter);
    float iAA = 0;
    float oAA = iAA;
    iAA += particle.innerSoftness;
    oAA += particle.outerSoftness;

    float innerColorVal = cutOff * circleSDF(dist, iAA, float(particle.radius));
    float borderColorVal = cutOff * circleSDF(dist, oAA, float(particle.radius) + float(particle.borderSize));

    borderColorVal = max(borderColorVal, innerColorVal);
    borderColorVal -= innerColorVal;

    sdfs[int(mix(PARTICLE_UP_INNER_SDF, PARTICLE_DOWN_INNER_SDF, particleSelector))] = innerColorVal;
    sdfs[int(mix(PARTICLE_UP_OUTER_SDF, PARTICLE_DOWN_OUTER_SDF, particleSelector))] = borderColorVal;

    bool useConnectors = (particle.connector.left.enable + particle.connector.right.enable) >= 1;

    (useConnectors ? connectorSDF(particleSelector) : (0));

    vec4 bC = vec4(particle.borderColor.xyz * particle.borderColor.w, particle.borderColor.w);
    vec4 iC = vec4(particle.color.xyz * particle.color.w, particle.color.w);
    vec4 cC = vec4(particle.connector.left.color.xyz * particle.connector.left.color.w, particle.connector.left.color.w);
    vec4 cBC = vec4(particle.connector.left.borderColor.xyz * particle.connector.left.borderColor.w, particle.connector.left.borderColor.w);
    vec4 cBRC = vec4(particle.connector.right.borderColor.xyz * particle.connector.right.borderColor.w, particle.connector.right.borderColor.w);
    vec4 cRC = vec4(particle.connector.right.color.xyz * particle.connector.right.color.w, particle.connector.right.color.w);

    topParticleBorderColor = mix(bC, topParticleBorderColor, particleSelector);
    topParticleColor = mix(iC, topParticleColor, particleSelector);

    topParticleConnectorColor = mix(cC, topParticleConnectorColor, particleSelector);
    topParticleConnectorBorderColor = mix(cBC, topParticleConnectorBorderColor, particleSelector);

    topParticleRightConnectorColor = mix(cRC, topParticleRightConnectorColor, particleSelector);
    topParticleRightConnectorBorderColor = mix(cBRC, topParticleRightConnectorBorderColor, particleSelector);

    return 0;
}

float getAudioVal(float n, float lastN, float isLeftSide)
{

    float audioX = n / lastN;

    int combineChannels = audioSettings.combineChannels;

    float audioVal = clamp(isLeftSide + (combineChannels), 0, 1) * texture(audioL, mix(audioX, 1. - audioX, audioSettings.reverseLeft)).x;
    audioVal += clamp(1 - isLeftSide + (combineChannels), 0, 1) * texture(audioR, mix(audioX, 1. - audioX, audioSettings.reverseRight)).x;
    audioVal /= 1. + (combineChannels);

    audioFetch(audioVal, n, lastN);
    return audioVal;
}

void defaultCapValues(inout Cap cap)
{
    cap.enable = false;
    cap.rate = 0;
    cap.audio = vec2(0);
    cap.launchVelocity = 0;
    cap.launchFlingMultiplier = 0;
    cap.acceleration = 0;
    cap.dragFactor = 0;
    cap.elasticity = 0;
    cap.elasticityMinThreshold = 0;
    cap.size = vec2(0);
    cap.offset = vec2(0);
    cap.color = vec4(0);
    cap.softness = vec3(0);
}

void defaultConnectorHalfValues(inout ConnectorHalf connector)
{
    connector.innerSoftness = 0;
    connector.outerSoftness = 0;

    connector.enable = 0;
    connector.height = 0;
    connector.borderSize = 0;
    connector.color = vec4(0);
    connector.borderColor = vec4(0);
}

void defaultConnectorValues(inout Connector connector)
{

    connector.jointMode = 0;
    connector.jointColorMode = 0;

    connector.leftCenter = vec2(0);
    connector.rightCenter = vec2(0);
    connector.currentCenter = vec2(0);

    defaultConnectorHalfValues(connector.left);
    defaultConnectorHalfValues(connector.right);
}

void defaultAudioSettingsValues()
{
    audioSettings.reverseLeft = 0;
    audioSettings.reverseRight = 0;
    audioSettings.mode = 1;
    audioSettings.combineChannels = 0;
}

void defaultAudioValues(inout Audio audio)
{
    audio.multiplier = 0;
    audio.prev = vec2(0);
    audio.current = vec2(0);
    audio.next = vec2(0);
}

void defaultParticleValues()
{
    particle.radius = (0);
    particle.borderSize = (0);

    particle.offset = vec2(0);

    particle.reverseBottomOffset = 0;
    particle.interChannelDistance = 0;

    particle.innerSoftness = (0);
    particle.outerSoftness = (0);

    particle.color = vec4(0);
    particle.borderColor = vec4(0);

    defaultAudioValues(particle.audio);
    defaultCapValues(particle.cap);
    defaultConnectorValues(particle.connector);
}

void defaultBarValues()
{
    bar.size = vec3(0);
    bar.borderSize = vec3(0);

    bar.offset = vec2(0);

    bar.innerSoftness = vec3(0);
    bar.outerSoftness = vec3(0);

    bar.color = vec4(0);
    bar.borderColor = vec4(0);
    bar.bgColor = vec4(0);

    bar.type = 0;

    bar.mergeLeftBar = 0;
    bar.mergeRightBar = 0;
    bar.clampLeftMergeBorder = 1;
    bar.clampRightMergeBorder = 1;

    defaultCapValues(bar.upCap);
    defaultCapValues(bar.downCap);
}

float heightSDF(vec2 p, float hAA, float hDAA, float vD, float h, float h2, vec2 c, float yOffset)
{

    float offset = yOffset;

    float vVal = applySoftness(hAA, h, vD - offset);
    vVal *= applySoftness(hDAA, h2 + hDAA, hDAA - vD + offset);
    return vVal;
}

vec2 smoothBarSDF(vec2 p, float wAA, float hD, float w, float hAA, float hDAA, float vD, float h, float h2, vec2 c, float yOffset)
{

    float hVal = (applySoftness((wAA), w, hD));

    return vec2(hVal, heightSDF(p, hAA, hDAA, vD, h, h2, c, yOffset));
}

float[4] vToArr(vec4 v)
{
    float[4] arr = { v.x, v.y, v.z, v.w };
    return arr;
}

vec4 colors[19];

vec4 getAllColors()
{

    colors[0] = (vec4(bar.bgColor.xyz * bar.bgColor.w, bar.bgColor.w));
    colors[1] = (vec4(bar.color.xyz * bar.color.w, bar.color.w));
    colors[2] = (vec4(bar.borderColor.xyz * bar.borderColor.w, bar.borderColor.w));
    colors[3] = (vec4(bar.upCap.color.xyz * bar.upCap.color.w, bar.upCap.color.w));
    colors[4] = (vec4(bar.downCap.color.xyz * bar.downCap.color.w, bar.downCap.color.w));
    colors[5] = (topParticleColor);
    colors[6] = (topParticleBorderColor);
    colors[7] = (topParticleConnectorColor);
    colors[8] = (topParticleRightConnectorColor);
    colors[9] = (topParticleConnectorBorderColor);
    colors[10] = (topParticleRightConnectorBorderColor);
    colors[11] = (topParticleCapColor);
    colors[12] = vec4(particle.color.xyz * particle.color.w, particle.color.w);
    colors[13] = vec4(particle.borderColor.xyz * particle.borderColor.w, particle.borderColor.w);
    colors[14] = (vec4(particle.connector.left.color.xyz * particle.connector.left.color.w, particle.connector.left.color.w));
    colors[15] = (vec4(particle.connector.right.color.xyz * particle.connector.right.color.w, particle.connector.right.color.w));
    colors[16] = (vec4(particle.connector.left.borderColor.xyz * particle.connector.left.borderColor.w, particle.connector.left.borderColor.w));
    colors[17] = (vec4(particle.connector.right.borderColor.xyz * particle.connector.right.borderColor.w, particle.connector.right.borderColor.w));
    colors[18] = (vec4(particle.cap.color.xyz * particle.cap.color.w, particle.cap.color.w));

    vec4 color = colors[zOrder[0]] * sdfs[zOrder[0]];

#define applyColors(i) color = addColors(colors[zOrder[i + 1]] * sdfs[zOrder[i + 1]], color);
#expand applyColors 18

    return color;
}

int rectangularBarSDF(vec2 prevOffset, vec2 currentOffset, vec2 nextOffset, vec2 pBS, vec2 nBS)
{
    vec2 barBottomCenter = vec2(bar.fragment.centerCoords.x, r_resolution.y / 2.), p = bar.fragment.coords;

    vec3 innerSoftness = bar.innerSoftness, outerSoftness = bar.outerSoftness, bS = bar.size, bOS = bar.borderSize;

    float clampLeftBarSide = mix(1., 1 - (step(bar.fragment.n, 0)), float(bar.clampLeftMergeBorder)), clampRightBarSide = mix(1., 1 - (step(bar.fragment.lastN, bar.fragment.n)), float(bar.clampRightMergeBorder));

    float pBH = bar.audio.next.y + clampRightBarSide * nBS.x, bH = bar.audio.current.y + bS.x;
    float nBH = bar.audio.prev.y + clampLeftBarSide * pBS.x;
    float barSpan = bar.fragment.span;
    float hD = abs(barSpan);
    float vD = (p.y - barBottomCenter.y - bOS.z);

    float h2 = (bar.audio.current.x + bS.z);
    float pH2 = bar.audio.next.x + clampRightBarSide * nBS.y;
    float nH2 = bar.audio.prev.x + clampLeftBarSide * pBS.y;

    int mergeBars = sign(bar.mergeLeftBar + bar.mergeRightBar);

    vec2 innerBarSDF = cutOff * smoothBarSDF(p, innerSoftness.y, hD, float(bS.y) / 2., innerSoftness.x, innerSoftness.z, vD, (bH - bOS.x - bOS.z), h2, barBottomCenter, currentOffset.y + bar.offset.y);
    vec2 prevInnerBarSDF = cutOff * (bar.mergeRightBar * vec2(0, heightSDF(p, innerSoftness.x, innerSoftness.z, vD, (pBH - bOS.x - bOS.z), pH2, barBottomCenter, prevOffset.y + bar.offset.y)));
    vec2 nextInnerBarSDF = cutOff * (bar.mergeLeftBar * vec2(0, heightSDF(p, innerSoftness.x, innerSoftness.z, vD, (nBH - bOS.x - bOS.z), nH2, barBottomCenter, nextOffset.y + bar.offset.y)));

    innerBarSDF.x = max(0, innerBarSDF.x);
    float smoothBarSpan = innerBarSDF.x, smoothBarHeight = innerBarSDF.y;

    vD = (p.y - barBottomCenter.y);

    vec2 outerBarSDF = cutOff * smoothBarSDF(p, outerSoftness.y, hD, float(bS.y) / 2. + bOS.y, outerSoftness.x, outerSoftness.z, vD, (bH), h2, barBottomCenter, currentOffset.y + bar.offset.y);
    vec2 prevOuterBarSDF = cutOff * (bar.mergeRightBar * vec2(0, heightSDF(p, outerSoftness.x, outerSoftness.z, vD, (pBH), pH2, barBottomCenter, prevOffset.y + bar.offset.y)));
    vec2 nextOuterBarSDF = cutOff * (bar.mergeLeftBar * vec2(0, heightSDF(p, outerSoftness.x, outerSoftness.z, vD, (nBH), nH2, barBottomCenter, nextOffset.y + bar.offset.y)));

    outerBarSDF.x = max(outerBarSDF.x, innerBarSDF.x);
    outerBarSDF.x -= innerBarSDF.x;

    outerBarSDF.y = max(outerBarSDF.y, innerBarSDF.y);
    outerBarSDF.y -= innerBarSDF.y;

    float smoothBorderSpan = outerBarSDF.x, smoothBorderHeight = outerBarSDF.y;

    prevOuterBarSDF.y = max(prevOuterBarSDF.y, prevInnerBarSDF.y);
    prevOuterBarSDF.y -= prevInnerBarSDF.y;

    nextOuterBarSDF.y = max(nextOuterBarSDF.y, nextInnerBarSDF.y);
    nextOuterBarSDF.y -= nextInnerBarSDF.y;

    float prevBarSpan = sign(bar.mergeRightBar) * (1. - step((p.x), barBottomCenter.x));
    float nextBarSpan = sign(bar.mergeLeftBar) * step(p.x, barBottomCenter.x);

    float innerBar = cutOff
        * (smoothBarHeight * smoothBarSpan
            + (mergeBars
                * (1 - smoothBarSpan)
                * ((prevBarSpan)*min(prevInnerBarSDF.y, smoothBarHeight) + (nextBarSpan)*min(nextInnerBarSDF.y, smoothBarHeight))));

    float outerBar = cutOff
        * (smoothBorderHeight * (smoothBarSpan + smoothBorderSpan)
            + mix(((smoothBorderSpan)*max(0, smoothBarHeight - prevBarSpan * prevInnerBarSDF.y - nextBarSpan * nextInnerBarSDF.y)

                      + (1 - smoothBarSpan - smoothBorderSpan)
                          * ((prevBarSpan * min(smoothBarHeight + smoothBorderHeight, prevOuterBarSDF.y + prevInnerBarSDF.y)
                                 + nextBarSpan * min(smoothBarHeight + smoothBorderHeight, nextOuterBarSDF.y + nextInnerBarSDF.y))

                              - prevBarSpan * min(smoothBarHeight, prevInnerBarSDF.y)
                              - nextBarSpan * min(smoothBarHeight, nextInnerBarSDF.y))),
                smoothBorderSpan * smoothBarHeight, 1. - mergeBars));

    sdfs[BAR_INNER_SDF] = innerBar;
    sdfs[BAR_OUTER_SDF] = outerBar;

    sdfs[BAR_UP_CAP_SDF] = cutOff * (bar.upCap.enable ? capSDF(1, 0, barSpan, bar.upCap.audio.y, bS.x + bar.offset.y + currentOffset.y, bar.upCap) : 0);

    sdfs[BAR_DOWN_CAP_SDF] = cutOff * (bar.downCap.enable ? capSDF(1, 1, barSpan, bar.downCap.audio.x, bar.downCap.offset.y + bS.z - bar.offset.y - currentOffset.y, bar.downCap) : 0);

    return 0;
}

int particleDownProps(vec2 currentCenter)
{

    vec2 currentOffset = vec2(0), currentBarOffset = vec2(0);
    vec2 prevBarSizeOffset = vec2(0), currentBarSizeOffset = vec2(0), nextBarSizeOffset = vec2(0);

    setOffsets(1, currentOffset, currentBarOffset, currentBarSizeOffset, bar.audio.current, particle.audio.current, currentCenter.x, particle.fragment.n, particle.fragment.lastN);

    vec2 nextOffset = vec2(0), nextBarOffset = vec2(0);
    setOffsets(1, nextOffset, nextBarOffset, nextBarSizeOffset, bar.audio.next, particle.audio.next, particle.connector.rightCenter.x, particle.fragment.n + 1, particle.fragment.lastN);

    vec2 prevOffset = vec2(0), prevBarOffset = vec2(0);
    setOffsets(1, prevOffset, prevBarOffset, prevBarSizeOffset, bar.audio.prev, particle.audio.prev, particle.connector.leftCenter.x, particle.fragment.n - 1, particle.fragment.lastN);

    setParticleDownProps();

    currentOffset.x += particle.offset.x;
    particle.fragment.centerCoords.x += particle.offset.x;
    particle.fragment.span -= particle.offset.x;

    int mirror = particle.reverseBottomOffset * 2 - 1;
    float pICD2 = mirror * particle.interChannelDistance / 2.;
    currentOffset.y += pICD2;
    prevOffset.y += pICD2;
    nextOffset.y += pICD2;

    float n = particle.fragment.n;
    float lastN = particle.fragment.lastN;

    float audio2 = particle.audio.current.x;

    particle.connector.rightCenter.y = r_resolution.y / 2 + particle.offset.y - mirror * nextOffset.y - (particle.audio.next.x);
    particle.connector.leftCenter.y = r_resolution.y / 2 + particle.offset.y - mirror * prevOffset.y - (particle.audio.prev.x);
    // particle.connector.rightCenter.x += nextOffset.x;
    // particle.connector.leftCenter.x += prevOffset.x;

    particle.offset.y = particle.offset.y - mirror * (currentOffset.y);

    particle.audio.current.y = -audio2;
    particle.connector.currentCenter = vec2(particle.fragment.centerCoords.x, particle.audio.current.y + particle.offset.y + r_resolution.y / 2.);

    particleSDF(1);

    sdfs[PARTICLE_DOWN_CAP_SDF] = cutOff
        * (particle.cap.enable
                ? capSDF(0, 1, particle.fragment.span, particle.cap.audio.x, -particle.offset.y + (particle.radius + particle.borderSize) + particle.cap.offset.y, particle.cap)
                : 0);

    return 0;
}

int roundedBarSDF(vec2 currentBarOffset)
{
    vec2 barBottomCenter = vec2(bar.fragment.centerCoords.x, r_resolution.y / 2. + currentBarOffset.y + bar.offset.y - bar.audio.current.x);
    float bH = bar.audio.current.x + bar.audio.current.y + bar.size.x;
    vec2 p = bar.fragment.coords;
    vec3 bS = bar.size;
    vec3 bOS = bar.borderSize;
    float barSpan = bar.fragment.span;

    float offset = (0);
    float S = halfConnectorSDF(p, vec2(barBottomCenter.x, barBottomCenter.y - bar.size.z), barBottomCenter + vec2(0, bH + offset));

    float W = bS.y / 2. + bOS.y;
    W /= 2.;
    float outerBar = S - W;

    float oAA = 0;
    float iAA = oAA;

    oAA += bar.outerSoftness.y;
    iAA += bar.innerSoftness.y;

    float col1 = (applySoftness(oAA, W, (outerBar)));

    W -= bOS.y;
    float innerBar = S - W;

    float col2 = (applySoftness(iAA, W, (innerBar)));

    col1 = max(col1, col2);
    col1 -= col2;

    sdfs[BAR_INNER_SDF] = cutOff * col2;
    sdfs[BAR_OUTER_SDF] = cutOff * col1;

    sdfs[BAR_UP_CAP_SDF] = cutOff * (bar.upCap.enable ? capSDF(1, 0, barSpan, bar.upCap.audio.y, (bar.offset.y + bar.size.x + currentBarOffset.y + bar.borderSize.y + bar.size.y / 2.), bar.upCap) : (0));

    sdfs[BAR_DOWN_CAP_SDF] = cutOff * (bar.downCap.enable ? capSDF(1, 1, barSpan, bar.downCap.audio.x, (-bar.offset.y - currentBarOffset.y + bar.downCap.offset.y) + bar.borderSize.y + bar.size.y / 2. + bar.size.z, bar.downCap) : 0);

    return 0;
}

void main()
{

    FragColor = vec4(0);

    Cap up, down;
    bar.upCap = up;
    bar.downCap = down;

    Audio audio;

    Connector connector;

    particle.audio = audio;
    particle.connector = connector;

    defaultParticleValues();

    defaultBarValues();

    defaultAudioSettingsValues();

    r_gl_FragCoord.x -= (leftPadding);

    int n = int(r_gl_FragCoord.x) / (fragmentWidth);
    int lastN = int(r_resolution.x - (leftPadding) - (rightPadding)) / (fragmentWidth)-1;
    float lastXCoords = getNthCenter(lastN).x;

    n = clamp(n, 0, lastN);
    cutOff = step(n, lastN);
    vec2 currentCenter = getNthCenter(n);

    Fragment fragment;

    fragment.span = r_gl_FragCoord.x - currentCenter.x; // RO
    fragment.n = n; // RO
    fragment.lastN = lastN; // RO

    fragment.coords = vec2(r_gl_FragCoord.xy);

    fragment.centerCoords = (currentCenter);

    particle.fragment = fragment;

    bar.fragment = particle.fragment;
    bar.audio = particle.audio;

    init();

    float audioMultiplier = particle.audio.multiplier;

    float mode = (1. - audioSettings.mode);

    float nextN = n + 1;
    float prevN = n - 1;

    float lastN2 = float(lastN) / 2.;

    float isLeftSide = mode * step(float(n), lastN2);
    float isNextLeftSide = mode * step(nextN, lastN2);
    float isPrevLeftSide = mode * step(prevN, lastN2);

    float topDirectionSelector = step(2, float(visualiserDirections)) + step(float(visualiserDirections), 0.);

    float bottomDirectionSelector = step(1., float(visualiserDirections));

    float particleSelector = step(2, float(visualiserMode)) + step(float(visualiserMode), 0);
    float barSelector = step(1, float(visualiserMode));

    float currentAudioVal = topDirectionSelector * getAudioVal(abs((mode)*lastN / 2 - n), lastN / (1. + mode), isLeftSide * mode);

    float nextAudioVal = sign(particle.connector.right.enable + bar.mergeRightBar) * topDirectionSelector * getAudioVal(abs((mode)*lastN / 2 - nextN), lastN / (1. + mode), isNextLeftSide * mode),
          prevAudioVal = sign(particle.connector.left.enable + bar.mergeLeftBar) * topDirectionSelector * getAudioVal(abs((mode)*lastN / 2 - prevN), lastN / (1. + mode), isPrevLeftSide * mode);

    particle.audio.current.y = particleSelector * audioMultiplier * (currentAudioVal);
    particle.audio.next.y = particleSelector * (particle.connector.right.enable * audioMultiplier * (nextAudioVal));
    particle.audio.prev.y = particleSelector * (particle.connector.left.enable * audioMultiplier * (prevAudioVal));

    float currentAudioLVal = bottomDirectionSelector * getAudioVal(abs((mode)*lastN / 2 - n), lastN / (1. + mode), isLeftSide + (1 - mode) * (1 - isLeftSide));
    float nextAudioLVal = sign(particle.connector.right.enable + bar.mergeRightBar) * bottomDirectionSelector * getAudioVal(abs((mode)*lastN / 2. - nextN), lastN / (1. + mode), isNextLeftSide + (1. - mode) * (1. - isNextLeftSide));
    float prevAudioLVal = sign(particle.connector.left.enable + bar.mergeLeftBar) * bottomDirectionSelector * getAudioVal(abs((mode)*lastN / 2. - prevN), lastN / (1. + mode), isPrevLeftSide + (1. - mode) * (1. - isPrevLeftSide));

    float clampLeftBarSide = mix(1., 1 - (step(n, 0)), float(bar.clampLeftMergeBorder)), clampRightBarSide = mix(1., 1 - (step(lastN, n)), float(bar.clampRightMergeBorder));

    particle.audio.current.x = particleSelector * audioMultiplier * currentAudioLVal;
    particle.audio.prev.x = particleSelector * (particle.connector.left.enable * audioMultiplier * prevAudioLVal);
    particle.audio.next.x = particleSelector * (particle.connector.right.enable * audioMultiplier * nextAudioLVal);

    bar.audio.current.y = barSelector * (bar.audio.multiplier) * currentAudioVal;
    bar.audio.next.y = barSelector * (bar.mergeRightBar * clampRightBarSide * (bar.audio.multiplier) * nextAudioVal);
    bar.audio.prev.y = barSelector * (bar.mergeLeftBar * clampLeftBarSide * (bar.audio.multiplier) * prevAudioVal);

    bar.audio.current.x = barSelector * bar.audio.multiplier * currentAudioLVal;
    bar.audio.prev.x = barSelector * (bar.mergeLeftBar * clampLeftBarSide * bar.audio.multiplier * prevAudioLVal);
    bar.audio.next.x = barSelector * (bar.mergeRightBar * clampRightBarSide * bar.audio.multiplier * nextAudioLVal);

    mat3x2 originalBarAudios = mat3x2(bar.audio.prev, bar.audio.current, bar.audio.next);

    mat3x2 exchangeBarChannels = mat3x2(step(bar.audio.prev, vec2(0)),
        step(bar.audio.current, vec2(0)),
        step(bar.audio.next, vec2(0)));

    bar.audio.prev -= dot(exchangeBarChannels[0], originalBarAudios[0]);
    bar.audio.current -= dot(exchangeBarChannels[1], originalBarAudios[1]);
    bar.audio.next -= dot(exchangeBarChannels[2], originalBarAudios[2]);

    vec2 currentOffset = vec2(0), currentBarOffset = vec2(0);
    vec2 prevBarSizeOffset = vec2(0), currentBarSizeOffset = vec2(0), nextBarSizeOffset = vec2(0);

    setOffsets(0, currentOffset, currentBarOffset, currentBarSizeOffset, bar.audio.current, particle.audio.current, currentCenter.x, n, lastN);

    particle.connector.rightCenter = getNthCenter(n + 1);
    vec2 nextOffset = vec2(0), nextBarOffset = vec2(0);
    setOffsets(0, nextOffset, nextBarOffset, nextBarSizeOffset, bar.audio.next, particle.audio.next, particle.connector.rightCenter.x, n + 1, lastN);
    particle.connector.rightCenter.x += nextOffset.x;

    particle.connector.rightCenter.y += nextOffset.y;

    particle.connector.leftCenter = getNthCenter(n - 1);

    vec2 prevOffset = vec2(0), prevBarOffset = vec2(0);
    setOffsets(0, prevOffset, prevBarOffset, prevBarSizeOffset, bar.audio.prev, particle.audio.prev, particle.connector.leftCenter.x, n - 1, lastN);

    particle.connector.leftCenter.x += prevOffset.x;
    particle.connector.leftCenter.y += prevOffset.y;

    bar.fragment.centerCoords.x = currentCenter.x;

    currentCenter.x += currentOffset.x;
    particle.fragment.centerCoords.x = currentCenter.x;
    particle.fragment.span = particle.fragment.coords.x - particle.fragment.centerCoords.x;

    particle.connector.rightCenter.y += topDirectionSelector * (audioMultiplier * nextAudioVal);
    particle.connector.leftCenter.y += topDirectionSelector * (audioMultiplier * prevAudioVal);

    particle.connector.currentCenter = vec2(particle.fragment.centerCoords.x, particle.audio.current.y + r_resolution.y / 2.);

    sdfs[BAR_BG_SDF] = 1 - step((visualiserMode), 0);

    bar.upCap.audio = bar.audio.current;
    bar.downCap.audio = bar.audio.current;

    particle.cap.audio = particle.audio.current;
    bar.fragment.span = max(-fragmentWidth / 2, bar.fragment.span);
    particle.fragment.span = max(-fragmentWidth / 2, particle.fragment.span);

    setProps();

    currentCenter.x += particle.offset.x;
    particle.fragment.centerCoords.x = currentCenter.x;
    particle.fragment.span = particle.fragment.coords.x - particle.fragment.centerCoords.x;

    bar.fragment.span = max(-fragmentWidth / 2, bar.fragment.span);
    particle.fragment.span = max(-fragmentWidth / 2, particle.fragment.span);

    particle.connector.rightCenter.y += topDirectionSelector * (particle.interChannelDistance / 2. + particle.offset.y);
    particle.connector.leftCenter.y += topDirectionSelector * (particle.interChannelDistance / 2. + particle.offset.y);

    bar.fragment.centerCoords.x += bar.offset.x + currentBarOffset.x;
    bar.fragment.span = bar.fragment.coords.x - bar.fragment.centerCoords.x;

    particle.connector.currentCenter = vec2(particle.fragment.centerCoords.x, particle.audio.current.y + particle.offset.y + currentOffset.y + particle.interChannelDistance / 2. + r_resolution.y / 2.);

    particleSelector* topDirectionSelector != 0 ? particleSDF(0) : 0;

    sdfs[PARTICLE_UP_CAP_SDF] = cutOff
        * (particle.cap.enable && topDirectionSelector * particleSelector != 0
                ? capSDF(0, 0, particle.fragment.span, particle.cap.audio.y, (particle.radius + particle.borderSize) + particle.offset.y + currentOffset.y + particle.interChannelDistance / 2., particle.cap)
                : 0);

    topParticleCapColor = mix(vec4(particle.cap.color.xyz * particle.cap.color.w, particle.cap.color.w), topParticleCapColor, 1. - particleSelector);

    vec2 prevBarSize = vec2(bar.size.x + prevBarSizeOffset.x, bar.size.z + prevBarSizeOffset.y);
    vec2 nextBarSize = vec2(bar.size.x + nextBarSizeOffset.x, bar.size.z + nextBarSizeOffset.y);
    bar.size.xz += currentBarSizeOffset;

    barSelector != 0 ? (bar.type == 1 ? roundedBarSDF(currentBarOffset)
                                      : rectangularBarSDF(nextBarOffset, currentBarOffset, prevBarOffset, prevBarSize, nextBarSize))
                     : 0;

    currentCenter.x -= particle.offset.x;
    particle.fragment.centerCoords.x = currentCenter.x;
    particle.fragment.span = particle.fragment.coords.x - particle.fragment.centerCoords.x;

    particleSelector* bottomDirectionSelector != 0
        ? particleDownProps(currentCenter)
        : 0;

    modifySDFs();

    FragColor = getAllColors();
}