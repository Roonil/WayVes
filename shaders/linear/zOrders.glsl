int colorBlendModes[19] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
int passThrough[17] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int layerOffsets[3] = { 0, 5, 12 }; // Bar, particle U, particle D

// for CBM, each parameter in the order corresponds to the z-index for that order.
// CBM 0 at ith position -> object at ith zIndex gets cut by the object on (i+1)th zIndex

// for PassThrough, ith position represents the ith z Index
// PassThrough 1 at ith Position -> if object at ith zIndex is cut, then so is the object at (i-1)th zIndex

// for setGroupZIndex, parameter i represents the ith zIndex.
// zIndex p at ith position -> pth object has zIndex i

void setBarGroupZIndex(int bgZ, int bIZ, int bOZ, int cUZ, int cDZ)
{
    int offset = layerOffsets[0];
    zOrder[offset] = bgZ;
    zOrder[offset + 1] = bIZ;
    zOrder[offset + 2] = bOZ;
    zOrder[offset + 3] = cUZ;
    zOrder[offset + 4] = cDZ;
}

void setBarGroupCBM(int bgZ, int bIZ, int bOZ, int cUZ, int cDZ)
{
    int offset = layerOffsets[0];
    colorBlendModes[offset] = bgZ;
    colorBlendModes[offset + 1] = bIZ;
    colorBlendModes[offset + 2] = bOZ;
    colorBlendModes[offset + 3] = cUZ;
    colorBlendModes[offset + 4] = cDZ;
}

void setBarGroupPassThrough(int a, int b, int c)
{
    int offset = layerOffsets[0];
    passThrough[offset] = a;
    passThrough[offset + 1] = b;
    passThrough[offset + 2] = c;
}

void setParticleUpGroupZIndex(int bgZ, int pIZ, int pOZ, int coLZ, int coRZ, int cUZ, int cDZ)
{
    int offset = layerOffsets[1];
    zOrder[offset] = 5 + bgZ;
    zOrder[offset + 1] = 5 + pIZ;
    zOrder[offset + 2] = 5 + pOZ;
    zOrder[offset + 3] = 5 + coLZ;
    zOrder[offset + 4] = 5 + coRZ;
    zOrder[offset + 5] = 5 + cUZ;
    zOrder[offset + 6] = 5 + cDZ;
}

void setParticleUpGroupCBM(int bgZ, int pIZ, int pOZ, int coLZ, int coRZ, int cUZ, int cDZ)
{
    int offset = layerOffsets[1];
    colorBlendModes[offset] = bgZ;
    colorBlendModes[offset + 1] = pIZ;
    colorBlendModes[offset + 2] = pOZ;
    colorBlendModes[offset + 3] = coLZ;
    colorBlendModes[offset + 4] = coRZ;
    colorBlendModes[offset + 5] = cUZ;
    colorBlendModes[offset + 6] = cDZ;
}

void setParticleUpGroupPassThrough(int a, int b, int c, int d, int e)
{
    int offset = layerOffsets[1];
    passThrough[offset] = a;
    passThrough[offset + 1] = b;
    passThrough[offset + 2] = c;
    passThrough[offset + 3] = d;
    passThrough[offset + 4] = e;
}

void setParticleDownGroupZIndex(int pIZ, int pOZ, int coLZ, int coRZ, int coLBZ, int coRBZ, int cZ)
{
    int offset = layerOffsets[2];
    zOrder[offset] = 12 + pIZ;
    zOrder[offset + 1] = 12 + pOZ;
    zOrder[offset + 2] = 12 + coLZ;
    zOrder[offset + 3] = 12 + coRZ;
    zOrder[offset + 4] = 12 + coLBZ;
    zOrder[offset + 5] = 12 + coRBZ;
    zOrder[offset + 6] = 12 + cZ;
}

void setParticleDownGroupCBM(int bgZ, int pIZ, int pOZ, int coLZ, int coRZ, int cUZ, int cDZ)
{
    int offset = layerOffsets[2];
    colorBlendModes[offset] = bgZ;
    colorBlendModes[offset + 1] = pIZ;
    colorBlendModes[offset + 2] = pOZ;
    colorBlendModes[offset + 3] = coLZ;
    colorBlendModes[offset + 4] = coRZ;
    colorBlendModes[offset + 5] = cUZ;
    colorBlendModes[offset + 6] = cDZ;
}

void setParticleDownGroupPassThrough(int a, int b, int c, int d, int e)
{
    int offset = layerOffsets[2];
    passThrough[offset] = a;
    passThrough[offset + 1] = b;
    passThrough[offset + 2] = c;
    passThrough[offset + 3] = d;
    passThrough[offset + 4] = e;
}

void setLayerOffsets(int barOffset, int pUOffset, int pDOffset)
{

    // 0,1,2
    // offsets=[0,5,12]

    // 0,2,1
    // offsets=[0,12,5]

    // 1,0,2
    // offsets=[7,0,12]

    // 1,2,0
    // offsets=[7,12,0]

    // 2,0,1
    // offsets=[14,0,7]

    // 2,1,0
    // offsets=[14,7,0]

    vec3 offsets = vec3(barOffset, pUOffset, pDOffset);

    mat3 steps = mat3(
        vec3(step(offsets, vec3(0))),
        vec3(step(offsets, vec3(1))),
        vec3(step(offsets, vec3(2))));

    mat3 layers = mat3(steps[0],
        (1. - steps[0]) * steps[1],
        (1 - steps[1]) * steps[2]);

    vec2 z = vec2(
        dot(layers[0], vec3(5, 7, 7)),
        dot(layers[1], vec3(5, 7, 7)));

    z.y += z.x;

    layerOffsets[0] = int(layers[1].x * z.x + layers[2].x * z.y);
    layerOffsets[1] = int(layers[1].y * z.x + layers[2].y * z.y);
    layerOffsets[2] = int(layers[1].z * z.x + layers[2].z * z.y);

    setBarGroupZIndex(0, 1, 2, 3, 4);
    setParticleUpGroupZIndex(0, 1, 2, 3, 4, 5, 6);
    setParticleDownGroupZIndex(0, 1, 2, 3, 4, 5, 6);
}

void applyZOrders()
{
    float[17] pSDFS;

    pSDFS[16] = passThrough[16] * sdfs[zOrder[18]];

#define addPSDFS(i) pSDFS[15 - i] = passThrough[15 - i] * (sdfs[zOrder[17 - i]] + pSDFS[16 - i]);
#expand addPSDFS 16

#define addSDFS(i) sdfs[zOrder[i]] = clamp(sdfs[zOrder[i]] - (1. - colorBlendModes[i]) * (sdfs[zOrder[i + 1]] + pSDFS[i]), 0, 1);
#expand addSDFS 17

    sdfs[zOrder[17]] -= (1. - colorBlendModes[17]) * sdfs[zOrder[18]];
    sdfs[zOrder[17]] = clamp(sdfs[zOrder[17]], 0, 1);
}