#version 430 core

precision highp float;

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

#define TWOPI 6.2831853071794
#define PI 3.1415926535897
#define RAD_PI PI / 180.

#include ":$CONFIGFILE"

vec4 topParticleColor = vec4(0), topParticleBorderColor = vec4(0),
     topParticleCapColor = vec4(0),
     topParticleConnectorColor = vec4(0), topParticleConnectorBorderColor = vec4(0),
     topParticleRightConnectorColor = vec4(0), topParticleRightConnectorBorderColor = vec4(0);

float applySoftness(float softness, float startVal, float endVal)
{
    return mix(step(endVal, startVal), 1. - smoothstep(startVal - softness, startVal, endVal), sign(softness));
}

float getAudioVal(float n, float lastN, float isLeftSide)
{

    float audioX = n / (lastN);

    int combineChannels = audioSettings.combineChannels;

    float audioVal = clamp(isLeftSide + (combineChannels), 0, 1) * texture(audioL, mix(1. - audioX, audioX, audioSettings.reverseLeft)).x;
    audioVal += clamp(1 - isLeftSide + (combineChannels), 0, 1) * texture(audioR, mix(1. - audioX, audioX, audioSettings.reverseRight)).x;
    audioVal /= 1. + (combineChannels);

    audioFetch(audioVal, n, lastN);
    return audioVal;
}

vec4 addColors(vec4 above, vec4 below)
{

    return above + (1 - above.w) * below;
}

float circleSDF(float d, float iAA, float r)
{

    float innerVal = mix(step(d, r), 1. - smoothstep(r - iAA, r, d), sign(iAA));
    return innerVal;
}

vec2 smoothCircleSDF(float l, float r, float iAA, float b, float oAA)
{
    vec2 sdf = vec2(0);

    sdf.x = circleSDF(l, iAA, r);
    sdf.y = circleSDF(l, oAA, r + b);
    sdf.y = max(sdf.y, sdf.x);
    sdf.y -= sdf.x;

    return sdf;
}
float halfConnectorSDF(vec2 pos, vec2 firstCenter, vec2 secondCenter)
{
    vec2 v1 = secondCenter - firstCenter;
    vec2 v2 = pos - firstCenter;
    float d = clamp(dot(v2, v1) / dot(v1, v1), 0.0, 1.0);
    return length(v2 - d * v1);
}

int connectorSDF(int particleDirectionSelector)
{
    vec2 pos = particle.fragment.vectorFromCenter;
    float isFirst = step(float(particle.fragment.n), 0.), isLast = step(float(particle.fragment.lastN), float(particle.fragment.n));

    float outerVal = halfConnectorSDF(pos, particle.connector.currentCenter, particle.connector.leftCenter);

    float d1 = outerVal;
    float W = particle.connector.left.height / 2. + particle.connector.left.borderSize;

    outerVal -= W;
    float oAA = 0;

    float iAA = oAA;
    iAA += particle.connector.left.innerSoftness;

    oAA += particle.connector.left.outerSoftness;

    float S = 1.;

    float col1 = S * (applySoftness((oAA), W, (outerVal)));

    outerVal = halfConnectorSDF(pos, particle.connector.rightCenter, particle.connector.currentCenter);
    float d2 = outerVal;
    W = particle.connector.right.height / 2. + particle.connector.right.borderSize;
    oAA = particle.connector.right.outerSoftness;
    outerVal -= W;

    float col2 = S * (applySoftness(oAA, W, (outerVal)));

    col2 = mix(mix(col2, 0, isFirst), col2, particle.connector.loop);
    col1 = mix(mix(col1, 0, isLast), col1, particle.connector.loop);

    float val = col1 + col2;

    int leftOuterSelector = int(mix(PARTICLE_UP_LEFT_CONNECTOR_OUTER_SDF, PARTICLE_DOWN_LEFT_CONNECTOR_OUTER_SDF, particleDirectionSelector));
    int rightOuterSelector = int(mix(PARTICLE_UP_RIGHT_CONNECTOR_OUTER_SDF, PARTICLE_DOWN_RIGHT_CONNECTOR_OUTER_SDF, particleDirectionSelector));

    int cutOff = int(step(particle.fragment.n, particle.fragment.lastN));
    sdfs[leftOuterSelector] = cutOff * col1;
    sdfs[rightOuterSelector] = cutOff * col2;

    val -= col1 * col2;

    float t1 = col1, t2 = col2;

    W = particle.connector.left.height / 2.;
    float innerVal = d1 - W;

    col1 = S * (applySoftness(iAA, W, (innerVal)));

    W = particle.connector.right.height / 2.;
    iAA = particle.connector.right.innerSoftness;
    innerVal = d2 - W;

    col2 = S * (applySoftness(iAA, W, (innerVal)));

    col2 = mix(mix(col2, 0, isFirst), col2, particle.connector.loop);
    col1 = mix(mix(col1, 0, isLast), col1, particle.connector.loop);

    int leftInnerSelector = int(mix(PARTICLE_UP_LEFT_CONNECTOR_INNER_SDF, PARTICLE_DOWN_LEFT_CONNECTOR_INNER_SDF, particleDirectionSelector));
    int rightInnerSelector = int(mix(PARTICLE_UP_RIGHT_CONNECTOR_INNER_SDF, PARTICLE_DOWN_RIGHT_CONNECTOR_INNER_SDF, particleDirectionSelector));

    sdfs[leftInnerSelector] = cutOff * (col1);
    sdfs[rightInnerSelector] = cutOff * (col2);

    t1 = sdfs[leftOuterSelector];
    t2 = sdfs[rightOuterSelector];

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

float capSDF(float mode, float direction, float dist, float barSpan, float audioVal, float yOffset, Cap cap)
{
    float directionSelector = (2 * direction - 1);

    ivec2 imageCoords = ivec2((gl_FragCoord.xy));

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

    float height = applySoftness(cap.softness.x, cap.size.y / 2., dist - capVal + directionSelector * (cap.offset.y + yOffset + cap.size.y / 2.));
    height *= applySoftness(cap.softness.z, cap.size.y / 2., -(dist - capVal + directionSelector * (cap.offset.y + yOffset + cap.size.y / 2.)));

    float currentHalfAL = (barSpan - cap.offset.x) * bar.fragment.distanceFromCenter;

    float width = applySoftness(cap.softness.y * mix(1., float(circle.radius), cap.type), cap.size.x / 2., mix(abs(barSpan - cap.offset.x), abs(currentHalfAL) - cap.size.x / 2. * circle.radius, cap.type));

    return width * height;
}

int roundedBarsSDF(vec2 currentCenter, vec2 barBottomCoords, vec2 barOffset)
{
    vec2 pos = bar.fragment.vectorFromCenter;
    float outerVal = halfConnectorSDF(pos, currentCenter, barBottomCoords);

    float innerVal = outerVal;

    float W = bar.size.y / 2. + bar.borderSize.y;

    outerVal -= W;
    float oAA = (fwidth(length(pos)));

    float iAA = oAA;

    iAA += bar.innerSoftness.y;

    oAA += bar.outerSoftness.y;

    float col1 = (applySoftness((oAA), W, (outerVal)));
    W = bar.size.y / 2.;
    innerVal -= W;

    float col2 = (applySoftness(iAA, W, (innerVal)));
    col1 = max(col1, col2);

    col1 -= col2;

    float cutOff = step(float(bar.fragment.n), float(bar.fragment.lastN));
    sdfs[BAR_BG_SDF] = cutOff;

    sdfs[BAR_UP_CAP_SDF] = cutOff * (bar.upCap.enable ? capSDF(1, 0, bar.fragment.distanceFromCenter - circle.radius - bar.size.y - 2 * bar.borderSize.y - barOffset.y, bar.fragment.span - barOffset.x, bar.upCap.audio.y, bar.size.x, bar.upCap) : 0);

    sdfs[BAR_DOWN_CAP_SDF] = cutOff * (bar.downCap.enable ? capSDF(1, 1, bar.fragment.distanceFromCenter - circle.radius - circle.borderSize + bar.size.y + 2 * bar.borderSize.y - barOffset.y, bar.fragment.span - barOffset.x, bar.downCap.audio.x, bar.downCap.offset.y + bar.size.z, bar.downCap) : 0);

    sdfs[BAR_INNER_SDF] = cutOff * col2;
    sdfs[BAR_OUTER_SDF] = cutOff * col1;

    return 0;
}

int barSDF(vec2 prevOffset, vec2 currentOffset, vec2 nextOffset, vec2 pBS, vec2 nBS)
{

    float angleSpan = bar.fragment.span - currentOffset.x;
    float vD = bar.fragment.distanceFromCenter;

    float clampLeftBarSide = mix(1., 1. - step(float(bar.fragment.lastN), float(bar.fragment.n)), float(bar.clampLeftMergeBorder)), clampRightBarSide = mix(1., 1. - step(float(bar.fragment.n), 0.), float(bar.clampRightMergeBorder));

    float H = currentOffset.y + circle.radius + circle.borderSize + bar.size.x + bar.audio.current.y;
    float pH = prevOffset.y + circle.radius + circle.borderSize + bar.audio.prev.y + pBS.x;
    float nH = nextOffset.y + circle.radius + circle.borderSize + bar.audio.next.y + nBS.x;

    float h2 = bar.audio.current.x + bar.size.z - currentOffset.y;
    float pH2 = bar.audio.next.x - prevOffset.y + nBS.y;
    float nH2 = bar.audio.prev.x - nextOffset.y + pBS.y;

    float barAngle = bar.size.y;

    vec3 innerSoftness = bar.innerSoftness;
    vec3 outerSoftness = bar.outerSoftness;

    float cutOff = step(float(bar.fragment.n), float(bar.fragment.lastN));
    sdfs[BAR_BG_SDF] = cutOff;

    // angleSpan = max(angleSpan, -fragmentAngle / 2.);

    float currentHalfAL = angleSpan * bar.fragment.distanceFromCenter;

    float smoothAngleSpan = cutOff
        * (mix(applySoftness(innerSoftness.y, float(barAngle) / 2., abs(angleSpan)),
            applySoftness(circle.radius * innerSoftness.y, circle.radius * innerSoftness.y, abs(currentHalfAL) - ((barAngle) / 2.) * circle.radius), bar.type));

    float smoothBorderSpan = cutOff
        * (mix(
            applySoftness(outerSoftness.y, float(barAngle) / 2. + bar.borderSize.y, abs(angleSpan)),
            applySoftness(circle.radius * outerSoftness.y, circle.radius * outerSoftness.y, abs(currentHalfAL) - ((barAngle) / 2. + bar.borderSize.y) * circle.radius),
            bar.type));

    smoothBorderSpan = max(smoothBorderSpan, smoothAngleSpan);
    smoothBorderSpan -= smoothAngleSpan;

    float smoothBorderHeight = (applySoftness(outerSoftness.x, H, vD));

    smoothBorderHeight *= applySoftness(outerSoftness.z, h2 + outerSoftness.z, outerSoftness.z - vD + circle.radius);

    float prevSmoothBorderHeight = (applySoftness(outerSoftness.x, pH, vD));

    prevSmoothBorderHeight *= applySoftness(outerSoftness.z, nH2 + outerSoftness.z, outerSoftness.z - vD + circle.radius);

    float nextSmoothBorderHeight = (applySoftness(outerSoftness.x, nH, vD));

    nextSmoothBorderHeight *= applySoftness(outerSoftness.z, pH2 + outerSoftness.z, outerSoftness.z - vD + circle.radius);

    float nextBarSpan = 1. - step(angleSpan, 0);

    vec4 bC = bar.color;
    bC.xyz *= bC.w;

    vec4 rBC = bar.borderColor;
    rBC.xyz *= rBC.w;

    vec4 cFC = circle.color;
    cFC.xyz *= cFC.w;

    vec4 cBC = circle.borderColor;
    cBC.xyz *= cBC.w;

    vD -= bar.borderSize.z;

    float smoothBarHeight = (applySoftness(innerSoftness.x, H - bar.borderSize.x - bar.borderSize.z, vD));

    smoothBarHeight *= applySoftness(bar.innerSoftness.z, h2 + bar.innerSoftness.z, bar.innerSoftness.z - vD + circle.radius);

    float prevSmoothBarHeight
        = (applySoftness(innerSoftness.x, pH - bar.borderSize.x - bar.borderSize.z, vD));

    prevSmoothBarHeight *= applySoftness(bar.innerSoftness.z, nH2 + bar.innerSoftness.z, bar.innerSoftness.z - vD + circle.radius);

    float nextSmoothBarHeight = (applySoftness(innerSoftness.x, nH - bar.borderSize.x - bar.borderSize.z, vD));

    nextSmoothBarHeight *= applySoftness(bar.innerSoftness.z, pH2 + bar.innerSoftness.z, bar.innerSoftness.z - vD + circle.radius);

    smoothBorderHeight = max(smoothBorderHeight, smoothBarHeight);
    smoothBorderHeight -= smoothBarHeight;

    prevSmoothBorderHeight = max(prevSmoothBorderHeight, prevSmoothBarHeight);
    prevSmoothBorderHeight -= prevSmoothBarHeight;

    nextSmoothBorderHeight = max(nextSmoothBorderHeight, nextSmoothBarHeight);
    nextSmoothBorderHeight -= nextSmoothBarHeight;

    prevSmoothBarHeight *= clampRightBarSide;
    nextSmoothBarHeight *= clampLeftBarSide;

    prevSmoothBorderHeight *= clampRightBarSide;
    nextSmoothBorderHeight *= clampLeftBarSide;

    int mergeBars = sign(bar.mergeLeftBar + bar.mergeRightBar);
    float innerBar = cutOff
        * (smoothBarHeight * smoothAngleSpan
            + (mergeBars
                * (1 - smoothAngleSpan)
                * mix(min(prevSmoothBarHeight, smoothBarHeight), min(nextSmoothBarHeight, smoothBarHeight), nextBarSpan)));

    float outerBar = cutOff
        * (smoothBorderHeight * (smoothAngleSpan + smoothBorderSpan)
            + mix(smoothBorderSpan * smoothBarHeight, ((smoothBorderSpan)*max(0, smoothBarHeight - mix(prevSmoothBarHeight, nextSmoothBarHeight, nextBarSpan))

                                                          + (1 - smoothAngleSpan - smoothBorderSpan) * (mix(min(smoothBarHeight + smoothBorderHeight, prevSmoothBorderHeight + prevSmoothBarHeight) - min(smoothBarHeight, prevSmoothBarHeight), min(smoothBarHeight + smoothBorderHeight, nextSmoothBorderHeight + nextSmoothBarHeight) - min(smoothBarHeight, nextSmoothBarHeight), nextBarSpan))),
                mergeBars));

    float upCapVal = cutOff * (bar.upCap.enable ? capSDF(1, 0, bar.fragment.distanceFromCenter - circle.radius, angleSpan, bar.upCap.audio.y, bar.size.x + currentOffset.y, bar.upCap) : 0);

    float downCapVal = cutOff * (bar.downCap.enable ? capSDF(1, 1, bar.fragment.distanceFromCenter - circle.radius, angleSpan, bar.downCap.audio.x, bar.downCap.offset.y + bar.size.z - currentOffset.y, bar.downCap) : 0);

    sdfs[BAR_INNER_SDF] = innerBar;
    sdfs[BAR_OUTER_SDF] = outerBar;

    sdfs[BAR_UP_CAP_SDF] = upCapVal;
    sdfs[BAR_DOWN_CAP_SDF] = downCapVal;

    return 0;
}

int particleSDF(int particleDirectionSelector)
{
    vec2 pos = particle.fragment.vectorFromCenter;

    float dist = length(pos - particle.connector.currentCenter);
    float iAA = fwidth((dist - particle.connector.currentCenter.x));
    float oAA = iAA;
    iAA += particle.innerSoftness;
    oAA += particle.outerSoftness;

    float innerColorVal = circleSDF(dist, iAA, float(particle.radius));
    float borderColorVal = circleSDF(dist, oAA, float(particle.radius) + float(particle.borderSize));

    borderColorVal = max(borderColorVal, innerColorVal);
    borderColorVal -= innerColorVal;

    bool useConnectors = (particle.connector.left.enable + particle.connector.right.enable) >= 1;

    (useConnectors ? connectorSDF(particleDirectionSelector) : (0));

    float cutOff = step(float(particle.fragment.n), float(particle.fragment.lastN));

    sdfs[int(mix(PARTICLE_UP_INNER_SDF, PARTICLE_DOWN_INNER_SDF, particleDirectionSelector))] = cutOff * innerColorVal;
    sdfs[int(mix(PARTICLE_UP_OUTER_SDF, PARTICLE_DOWN_OUTER_SDF, particleDirectionSelector))] = cutOff * borderColorVal;

    vec4 bC = vec4(particle.borderColor.xyz * particle.borderColor.w, particle.borderColor.w);
    vec4 iC = vec4(particle.color.xyz * particle.color.w, particle.color.w);
    vec4 cC = vec4(particle.connector.left.color.xyz * particle.connector.left.color.w, particle.connector.left.color.w);
    vec4 cBC = vec4(particle.connector.left.borderColor.xyz * particle.connector.left.borderColor.w, particle.connector.left.borderColor.w);
    vec4 cBRC = vec4(particle.connector.right.borderColor.xyz * particle.connector.right.borderColor.w, particle.connector.right.borderColor.w);
    vec4 cRC = vec4(particle.connector.right.color.xyz * particle.connector.right.color.w, particle.connector.right.color.w);

    topParticleBorderColor = mix(bC, topParticleBorderColor, particleDirectionSelector);
    topParticleColor = mix(iC, topParticleColor, particleDirectionSelector);

    topParticleConnectorColor = mix(cC, topParticleConnectorColor, particleDirectionSelector);
    topParticleConnectorBorderColor = mix(cBC, topParticleConnectorBorderColor, particleDirectionSelector);

    topParticleRightConnectorColor = mix(cRC, topParticleRightConnectorColor, particleDirectionSelector);
    topParticleRightConnectorBorderColor = mix(cBRC, topParticleRightConnectorBorderColor, particleDirectionSelector);

    return 0;
}

float getNthCenter(int n)
{
    return (2. * n + 1.) * ((fragmentAngle) / 2.);
}

void defaultAudioSettingsValues(inout AudioSettings audio)
{
    audio.reverseLeft = 0;
    audio.reverseRight = 0;
    audio.mode = 0;
    audio.combineChannels = 0;
}

void defaultAudioValues(inout Audio audio)
{
    audio.multiplier = 0;
    audio.current = vec2(0);
    audio.next = vec2(0);
    audio.prev = vec2(0);
}

void defaultCapValues(inout Cap cap)
{
    cap.enable = false;
    cap.rate = 0;
    cap.audio = vec2(0);
    cap.softness = vec3(0);
    cap.launchVelocity = 0;
    cap.launchFlingMultiplier = 0;
    cap.acceleration = 0;
    cap.dragFactor = 0;
    cap.elasticity = 0;
    cap.type = 0;
    cap.elasticityMinThreshold = 0;
    cap.size = vec2(0);
    cap.offset = vec2(0);
    cap.color = vec4(0);
}

void defaultConnectorHalfValues(inout ConnectorHalf connector)
{
    connector.enable = 0;

    connector.height = 0;
    connector.borderSize = 0;

    connector.innerSoftness = 0;
    connector.outerSoftness = 0;

    connector.color = vec4(0);
    connector.borderColor = vec4(0);
}

void defaultConnectorValues(inout Connector connector)
{

    connector.jointMode = 0;
    connector.jointColorMode = 0;
    connector.loop = 0;

    connector.mergeEnds = 0;

    connector.leftCenter = vec2(0);
    connector.rightCenter = vec2(0);
    connector.currentCenter = vec2(0);

    defaultConnectorHalfValues(connector.left);
    defaultConnectorHalfValues(connector.right);
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
    defaultConnectorValues(particle.connector);
    defaultCapValues(particle.cap);
}

void defaultCircleValues()
{
    circle.color = vec4(0);
    circle.borderColor = vec4(0);
    circle.radius = 0;

    circle.angleOffset = 0;
    circle.maxAngle = 360;
    circle.restrictCircleAngle = 1;

    circle.borderSize = 0;
    circle.innerSoftness = 0;
    circle.outerSoftness = 0;
    circle.center = vec2(r_resolution / 2.);
}

void defaultBarValues()
{
    bar.size.y = 0.;
    bar.bgColor = vec4(0);

    bar.mergeLeftBar = 0;
    bar.mergeRightBar = 0;
    bar.mergeEnds = 0;

    bar.type = 0;

    bar.innerSoftness = vec3(0);
    bar.outerSoftness = vec3(0);

    bar.offset = vec2(0);
    bar.color = vec4(0);
    bar.borderColor = vec4(0);
    bar.size = vec3(0);
    bar.borderSize = vec3(0);

    bar.clampLeftMergeBorder = 0;
    bar.clampRightMergeBorder = 0;

    defaultCapValues(bar.upCap);
    defaultCapValues(bar.downCap);
}

int particleDownProps(vec2 prevNorm, vec2 currentNorm, vec2 nextNorm, float actualCurrentCenterAngle, float prevN, float nextN, float cutOff)
{
    vec2 currentParticleOffset = vec2(0), prevParticleOffset = vec2(0), nextParticleOffset = vec2(0);
    vec2 currentBarOffset = vec2(0), prevBarOffset = vec2(0), nextBarOffset = vec2(0);
    vec2 prevBarSizeOffset = vec2(0), currentBarSizeOffset = vec2(0), nextBarSizeOffset = vec2(0);

    setOffsets(1., prevParticleOffset, prevBarOffset, prevBarSizeOffset, bar.audio.prev, particle.audio.prev, particle.connector.rightCenter.x, prevN, particle.fragment.lastN);
    setOffsets(1., currentParticleOffset, currentBarOffset, currentBarSizeOffset, bar.audio.current, particle.audio.current, particle.connector.currentCenter.x, particle.fragment.n, particle.fragment.lastN);
    setOffsets(1., nextParticleOffset, nextBarOffset, nextBarSizeOffset, bar.audio.next, particle.audio.next, particle.connector.leftCenter.x, nextN, particle.fragment.lastN);

    setParticleDownProps();
    currentParticleOffset += particle.offset;

    float currentOffsetX = currentParticleOffset.x;
    currentParticleOffset.x *= RAD_PI;

    float mirror = 1 - particle.reverseBottomOffset * 2;
    float pICD2 = mirror * particle.interChannelDistance / 2.;

    currentParticleOffset.y -= pICD2;
    nextParticleOffset.y -= pICD2;
    prevParticleOffset.y -= pICD2;

    currentNorm = vec2(cos(actualCurrentCenterAngle + currentParticleOffset.x), sin(actualCurrentCenterAngle + currentParticleOffset.x));

    particle.connector.currentCenter = ((circle.radius - particle.audio.current.x + mirror * currentParticleOffset.y)) * currentNorm;
    particle.connector.rightCenter = (circle.radius - particle.audio.prev.x + mirror * prevParticleOffset.y) * prevNorm;
    particle.connector.leftCenter = (circle.radius - particle.audio.next.x + mirror * nextParticleOffset.y) * nextNorm;

    particle.connector.currentCenter.x += currentParticleOffset.x;

    int pC2 = particleSDF(1);
    float downCapVal = (particle.cap.enable ? capSDF(0, 1, particle.fragment.distanceFromCenter - circle.radius + particle.radius + particle.borderSize - mirror * currentParticleOffset.y, particle.fragment.span - currentOffsetX, particle.cap.audio.x, 0, particle.cap) : 0);

    sdfs[PARTICLE_DOWN_CAP_SDF] = cutOff * downCapVal;

    return 0;
}

vec4 colors[21];

vec4 getAllColors()
{
    colors[0] = vec4(circle.color.xyz * circle.color.w, circle.color.w);
    colors[1] = vec4(circle.borderColor.xyz * circle.borderColor.w, circle.borderColor.w);
    colors[2] = vec4(bar.bgColor.xyz * bar.bgColor.w, bar.bgColor.w);
    colors[3] = vec4(bar.color.xyz * bar.color.w, bar.color.w);
    colors[4] = vec4(bar.borderColor.xyz * bar.borderColor.w, bar.borderColor.w);
    colors[5] = vec4(bar.upCap.color.xyz * bar.upCap.color.w, bar.upCap.color.w);
    colors[6] = vec4(bar.downCap.color.xyz * bar.downCap.color.w, bar.downCap.color.w);
    colors[7] = (topParticleColor);
    colors[8] = (topParticleBorderColor);
    colors[9] = (topParticleConnectorColor);
    colors[10] = (topParticleRightConnectorColor);
    colors[11] = (topParticleConnectorBorderColor);
    colors[12] = (topParticleRightConnectorBorderColor);
    colors[13] = vec4(topParticleCapColor);
    colors[14] = vec4(particle.color.xyz * particle.color.w, particle.color.w);
    colors[15] = vec4(particle.borderColor.xyz * particle.borderColor.w, particle.borderColor.w);
    colors[16] = vec4(particle.connector.left.color.xyz * particle.connector.left.color.w, particle.connector.left.color.w);
    colors[17] = vec4(particle.connector.right.color.xyz * particle.connector.right.color.w, particle.connector.right.color.w);
    colors[18] = vec4(particle.connector.left.borderColor.xyz * particle.connector.left.borderColor.w, particle.connector.left.borderColor.w);
    colors[19] = vec4(particle.connector.right.borderColor.xyz * particle.connector.right.borderColor.w, particle.connector.right.borderColor.w);
    colors[20] = vec4(particle.cap.color.xyz * particle.cap.color.w, particle.cap.color.w);

    vec4 color = colors[zOrder[0]] * sdfs[zOrder[0]];
#define applyColors(i) color = addColors(colors[zOrder[i + 1]] * sdfs[zOrder[i + 1]], color);
#expand applyColors 20

    return color;
}

void main()
{
    defaultAudioSettingsValues(audioSettings);

    defaultAudioValues(bar.audio);

    defaultParticleValues();
    defaultCircleValues();
    defaultBarValues();

    init();

    FragColor = vec4(0);
    vec2 vecFromCenter = r_gl_FragCoord.xy - circle.center;
    float angle = (circle.angleOffset) * RAD_PI;
    float angleSin = sin(angle), angleCos = cos(angle);
    vecFromCenter = mat2(vec2(angleSin, angleCos), vec2(-angleCos, angleSin)) * vecFromCenter;
    float currentAngle = atan(vecFromCenter.y, vecFromCenter.x) / PI;
    currentAngle += 1.;
    currentAngle *= 180.;

    int n = int(currentAngle / (fragmentAngle));
    int lastN = int(round((circle.maxAngle) / (fragmentAngle))) - 1;
    lastN = max(1, lastN);

    int actualN = n;
    n = min(n, lastN);
    float currentCenterAngle = getNthCenter(n);
    float dist = length(vecFromCenter);

    Fragment fragment;

    bar.fragment = fragment;
    bar.fragment.span = currentAngle - currentCenterAngle;
    bar.fragment.span = max(bar.fragment.span, -(fragmentAngle) / 2.);
    currentAngle *= RAD_PI;

    bar.fragment.coords = r_gl_FragCoord.xy;
    bar.fragment.currentAngle = currentAngle;
    bar.fragment.distanceFromCenter = dist;
    bar.fragment.currentCenterAngle = currentCenterAngle;
    bar.fragment.n = n;
    bar.fragment.lastN = lastN;

    particle.fragment = fragment;

    particle.fragment.currentAngle = currentAngle;
    particle.fragment.distanceFromCenter = dist;
    particle.fragment.n = n;
    particle.fragment.lastN = lastN;
    particle.fragment.span = bar.fragment.span;

    vecFromCenter = dist * vec2(cos(currentAngle), sin(currentAngle));
    particle.fragment.vectorFromCenter = vecFromCenter;

    bar.fragment.vectorFromCenter = vecFromCenter;

    float lastN2 = float(lastN) / 2.;
    float n1 = float(n);

    int nextN = int(mix((n + 1), 0, step(float(lastN), n1)));
    int prevN = int(mix((n - 1), lastN, step(n1, 0.)));

    float mode = (1. - audioSettings.mode);
    float isLeftSide = mode * step(n1, lastN2);
    float isNextLeftSide = mode * step(nextN, lastN2);
    float isPrevLeftSide = mode * step(prevN, lastN2);

    float topDirectionSelector = step(2, float(visualiserDirections)) + step(float(visualiserDirections), 0.);

    float bottomDirectionSelector = step(1., float(visualiserDirections));

    float particleSelector = step(2, float(visualiserMode)) + step(float(visualiserMode), 0);
    float barSelector = step(1, float(visualiserMode));

    float currentAudioVal = topDirectionSelector * (getAudioVal(abs((mode)*lastN / 2 - n), lastN / (1. + mode), isLeftSide * mode));
    float prevAudioVal = sign(bar.mergeRightBar + particle.connector.right.enable) * (topDirectionSelector * getAudioVal(abs((mode)*lastN / 2 - prevN), lastN / (1. + mode), isPrevLeftSide * mode));
    float nextAudioVal = sign(bar.mergeLeftBar + particle.connector.left.enable) * (topDirectionSelector * getAudioVal(abs((mode)*lastN / 2 - nextN), lastN / (1. + mode), isNextLeftSide * mode));

    float currentAudioLVal = bottomDirectionSelector * (getAudioVal(abs((mode)*lastN / 2 - n), lastN / (1. + mode), isLeftSide + (1 - mode) * (1 - isLeftSide)));
    float prevAudioLVal = sign(bar.mergeRightBar + particle.connector.right.enable) * bottomDirectionSelector * (getAudioVal(abs((mode)*lastN / 2 - prevN), lastN / (1. + mode), isPrevLeftSide + (1 - mode) * (1 - isPrevLeftSide)));
    float nextAudioLVal = sign(bar.mergeLeftBar + particle.connector.left.enable) * bottomDirectionSelector * (getAudioVal(abs((mode)*lastN / 2 - nextN), lastN / (1. + mode), isNextLeftSide + (1 - mode) * (1 - isNextLeftSide)));

    particle.audio.current.y = particle.audio.multiplier * currentAudioVal;
    particle.audio.prev.y = particle.connector.right.enable * (particle.audio.multiplier * prevAudioVal);
    particle.audio.next.y = particle.connector.left.enable * (particle.audio.multiplier * nextAudioVal);

    particle.audio.current.x = particle.audio.multiplier * currentAudioLVal;
    particle.audio.prev.x = particle.connector.right.enable * (particle.audio.multiplier * prevAudioLVal);
    particle.audio.next.x = particle.connector.left.enable * (particle.audio.multiplier * nextAudioLVal);

    bar.audio.current.y = bar.audio.multiplier * currentAudioVal;
    bar.audio.prev.y = bar.mergeRightBar * bar.audio.multiplier * prevAudioVal;
    bar.audio.prev.x = bar.mergeRightBar * bar.audio.multiplier * prevAudioLVal;
    bar.audio.next.y = bar.mergeLeftBar * bar.audio.multiplier * nextAudioVal;
    bar.audio.next.x = bar.mergeLeftBar * bar.audio.multiplier * nextAudioLVal;
    bar.audio.current.x = bar.audio.multiplier * currentAudioLVal;

    mat3x2 originalBarAudios = mat3x2(bar.audio.prev, bar.audio.current, bar.audio.next);

    mat3x2 exchangeBarChannels = mat3x2(step(bar.audio.prev, vec2(0)),
        step(bar.audio.current, vec2(0)),
        step(bar.audio.next, vec2(0)));

    bar.audio.prev -= dot(exchangeBarChannels[0], originalBarAudios[0]);
    bar.audio.current -= dot(exchangeBarChannels[1], originalBarAudios[1]);
    bar.audio.next -= dot(exchangeBarChannels[2], originalBarAudios[2]);

    bar.upCap.audio = bar.audio.current;
    bar.downCap.audio = bar.audio.current;
    particle.cap.audio = particle.audio.current;

    vec2 currentParticleOffset = vec2(0), prevParticleOffset = vec2(0), nextParticleOffset = vec2(0);
    vec2 currentBarOffset = vec2(0), prevBarOffset = vec2(0), nextBarOffset = vec2(0);

    vec2 prevBarSizeOffset = vec2(0), currentBarSizeOffset = vec2(0), nextBarSizeOffset = vec2(0);

    setOffsets(0., prevParticleOffset, prevBarOffset, prevBarSizeOffset, bar.audio.prev, particle.audio.prev, particle.connector.rightCenter.x, prevN, lastN);
    setOffsets(0., currentParticleOffset, currentBarOffset, currentBarSizeOffset, bar.audio.current, particle.audio.current, particle.connector.currentCenter.x, n, lastN);
    setOffsets(0., nextParticleOffset, nextBarOffset, nextBarSizeOffset, bar.audio.next, particle.audio.next, particle.connector.leftCenter.x, nextN, lastN);

    setProps();

    currentParticleOffset += particle.offset;
    currentBarOffset += bar.offset;
    particle.fragment.n = int(mix(actualN, n, particle.connector.mergeEnds));
    bar.fragment.n = int(mix(actualN, n, bar.mergeEnds));

    float iAA = circle.innerSoftness;
    float oAA = circle.outerSoftness;

    vec2 circleSDF = smoothCircleSDF(dist, circle.radius, iAA, circle.borderSize, oAA);

    float prevCenterAngle = (particle.connector.right.enable * (getNthCenter(prevN)));
    float nextCenterAngle = (particle.connector.left.enable * (getNthCenter(nextN)));

    float cutOff = step(actualN, float(particle.fragment.lastN));

    currentCenterAngle *= RAD_PI;

    float actualCurrentCenterAngle = currentCenterAngle;

    vec2 currentBarNorm = vec2(cos(currentCenterAngle + currentBarOffset.x * RAD_PI),
        sin((currentCenterAngle + currentBarOffset.x * RAD_PI)));

    prevCenterAngle += prevParticleOffset.x;
    currentCenterAngle += currentParticleOffset.x * RAD_PI;
    nextCenterAngle += nextParticleOffset.x;

    prevCenterAngle *= RAD_PI;
    nextCenterAngle *= RAD_PI;

    vec2 currentParticleNorm = vec2(cos(currentCenterAngle), sin(currentCenterAngle));
    vec2 prevNorm = vec2(cos(prevCenterAngle), sin(prevCenterAngle));
    vec2 nextNorm = vec2(cos(nextCenterAngle), sin(nextCenterAngle));

    particle.connector.currentCenter = (circle.radius + circle.borderSize + particle.audio.current.y) * currentParticleNorm;
    particle.connector.rightCenter = (circle.radius + circle.borderSize + particle.audio.prev.y) * prevNorm;
    particle.connector.leftCenter = (circle.radius + circle.borderSize + particle.audio.next.y) * nextNorm;

    float pICD2 = particle.interChannelDistance / 2.;
    particle.connector.rightCenter += (prevParticleOffset.y + pICD2) * prevNorm;
    particle.connector.currentCenter += (currentParticleOffset.y + pICD2) * currentParticleNorm;
    particle.connector.leftCenter += (nextParticleOffset.y + pICD2) * nextNorm;

    // prevParticleOffset.x *= RAD_PI;
    // currentParticleOffset.x *= RAD_PI;
    // nextParticleOffset.x *= RAD_PI;

    float H = (circle.radius + circle.borderSize + (bar.audio.current.y + bar.size.x) + currentBarOffset.y);
    float H2 = (circle.radius + circle.borderSize - (bar.size.z + bar.audio.current.x) + currentBarOffset.y);

    vec2 prevBarSize = vec2(bar.size.x + prevBarSizeOffset.x, bar.size.z + prevBarSizeOffset.y);
    vec2 nextBarSize = vec2(bar.size.x + nextBarSizeOffset.x, bar.size.z + nextBarSizeOffset.y);
    bar.size.xz += currentBarSizeOffset;

    int returnValue = (barSelector != 0
            ? (bar.type == 2
                      ? roundedBarsSDF(H * currentBarNorm,
                            H2 * currentBarNorm, currentBarOffset)
                      : barSDF(prevBarOffset, currentBarOffset, nextBarOffset, prevBarSize, nextBarSize))
            : (0));

    returnValue = particleSelector * topDirectionSelector != 0 ? particleSDF(0) : (0);
    float upCapVal = ((particleSelector * topDirectionSelector != 0) && particle.cap.enable ? capSDF(0, 0, particle.fragment.distanceFromCenter - circle.radius - particle.radius - particle.borderSize - currentParticleOffset.y - pICD2, particle.fragment.span - currentParticleOffset.x, particle.cap.audio.y, 0, particle.cap) : 0);

    sdfs[PARTICLE_UP_CAP_SDF] = cutOff * upCapVal;
    topParticleCapColor = mix(vec4(particle.cap.color.xyz * particle.cap.color.w, particle.cap.color.w), topParticleCapColor, 1. - particleSelector);

    vec4 cFC = circle.color;
    cFC.xyz *= cFC.w;

    vec4 cBC = circle.borderColor;
    cBC.xyz *= cBC.w;

    float cutOffCircle = mix(1., cutOff, circle.restrictCircleAngle);

    vec4 cC = (cutOffCircle) * (circleSDF.x * cFC + circleSDF.y * cBC);
    sdfs[CIRCLE_INNER_SDF] = (cutOffCircle)*circleSDF.x;
    sdfs[CIRCLE_OUTER_SDF] = (cutOffCircle)*circleSDF.y;

    returnValue = (particleSelector * bottomDirectionSelector != 0 ? particleDownProps(prevNorm, currentParticleNorm, nextNorm, actualCurrentCenterAngle, prevN, nextN, cutOff) : 0);
    modifySDFs();
    FragColor = getAllColors();
}
