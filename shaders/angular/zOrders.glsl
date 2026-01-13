int layerOffsets[4] = { 0, 2, 7, 14 };

int colorBlendModes[21] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
int passThrough[19] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

void setCircleGroupZIndex(int cIZ, int cOZ)
{
    int offset = layerOffsets[0];
    zOrder[offset] = cIZ;
    zOrder[offset + 1] = cOZ;
}

void setCircleGroupCBM(int cIZ, int cOZ)
{
    int offset = layerOffsets[0];
    colorBlendModes[offset] = cIZ;
    colorBlendModes[offset + 1] = cOZ;
}

void setBarGroupZIndex(int bgZ, int bIZ, int bOZ, int cUZ, int cDZ)
{
    int offset = layerOffsets[1];
    zOrder[offset] = 2 + bgZ;
    zOrder[offset + 1] = 2 + bIZ;
    zOrder[offset + 2] = 2 + bOZ;
    zOrder[offset + 3] = 2 + cUZ;
    zOrder[offset + 4] = 2 + cDZ;
}

void setBarGroupCBM(int bgZ, int bIZ, int bOZ, int cUZ, int cDZ)
{
    int offset = layerOffsets[1];
    colorBlendModes[offset] = bgZ;
    colorBlendModes[offset + 1] = bIZ;
    colorBlendModes[offset + 2] = bOZ;
    colorBlendModes[offset + 3] = cUZ;
    colorBlendModes[offset + 4] = cDZ;
}

void setBarGroupPassThrough(int a, int b, int c)
{
    int offset = layerOffsets[1];
    passThrough[offset] = a;
    passThrough[offset + 1] = b;
    passThrough[offset + 2] = c;
}

void setParticleUpGroupZIndex(int bgZ, int pIZ, int pOZ, int coLZ, int coRZ, int cUZ, int cDZ)
{
    int offset = layerOffsets[2];
    zOrder[offset] = 7 + bgZ;
    zOrder[offset + 1] = 7 + pIZ;
    zOrder[offset + 2] = 7 + pOZ;
    zOrder[offset + 3] = 7 + coLZ;
    zOrder[offset + 4] = 7 + coRZ;
    zOrder[offset + 5] = 7 + cUZ;
    zOrder[offset + 6] = 7 + cDZ;
}

void setParticleUpGroupCBM(int bgZ, int pIZ, int pOZ, int coLZ, int coRZ, int cUZ, int cDZ)
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

void setParticleUpGroupPassThrough(int a, int b, int c, int d, int e)
{
    int offset = layerOffsets[2];
    passThrough[offset] = a;
    passThrough[offset + 1] = b;
    passThrough[offset + 2] = c;
    passThrough[offset + 3] = d;
    passThrough[offset + 4] = e;
}

void setParticleDownGroupZIndex(int pIZ, int pOZ, int coLZ, int coRZ, int coLBZ, int coRBZ, int cZ)
{
    int offset = layerOffsets[3];
    zOrder[offset] = 14 + pIZ;
    zOrder[offset + 1] = 14 + pOZ;
    zOrder[offset + 2] = 14 + coLZ;
    zOrder[offset + 3] = 14 + coRZ;
    zOrder[offset + 4] = 14 + coLBZ;
    zOrder[offset + 5] = 14 + coRBZ;
    zOrder[offset + 6] = 14 + cZ;
}

void setParticleDownGroupCBM(int bgZ, int pIZ, int pOZ, int coLZ, int coRZ, int cUZ, int cDZ)
{
    int offset = layerOffsets[3];
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
    int offset = layerOffsets[3];
    passThrough[offset] = a;
    passThrough[offset + 1] = b;
    passThrough[offset + 2] = c;
    passThrough[offset + 3] = d;
    passThrough[offset + 4] = e;
}

void setLayerOffsets(int circleOffset, int barOffset, int pUOffset, int pDOffset)
{

    vec4 offsets = vec4(circleOffset, barOffset, pUOffset, pDOffset);

    mat4 steps = mat4(
        vec4(step(offsets, vec4(0))),
        vec4(step(offsets, vec4(1))),
        vec4(step(offsets, vec4(2))),
        vec4(step(offsets, vec4(3))));

    mat4 layers = mat4(steps[0],
        (1. - steps[0]) * steps[1],
        (1 - steps[1]) * steps[2],
        (1 - steps[2]) * steps[3]);

    vec3 z = vec3(
        dot(layers[0], vec4(2, 5, 7, 7)),
        dot(layers[1], vec4(2, 5, 7, 7)),
        dot(layers[2], vec4(2, 5, 7, 7)));

    z.y += z.x;
    z.z += z.y;

    layerOffsets[0] = int(layers[1].x * z.x + layers[2].x * z.y + layers[3].x * z.z);
    layerOffsets[1] = int(layers[1].y * z.x + layers[2].y * z.y + layers[3].y * z.z);
    layerOffsets[2] = int(layers[1].z * z.x + layers[2].z * z.y + layers[3].z * z.z);
    layerOffsets[3] = int(layers[1].w * z.x + layers[2].w * z.y + layers[3].w * z.z);

    setCircleGroupZIndex(0, 1);
    setBarGroupZIndex(0, 1, 2, 3, 4);
    setParticleUpGroupZIndex(0, 1, 2, 3, 4, 5, 6);
    setParticleDownGroupZIndex(0, 1, 2, 3, 4, 5, 6);
}

void applyZOrders()
{
    float[19] pSDFS;

    pSDFS[18] = passThrough[18] * sdfs[zOrder[20]];

#define addPSDFS(i) pSDFS[17 - i] = passThrough[17 - i] * (sdfs[zOrder[19 - i]] + pSDFS[18 - i]);
#expand addPSDFS 18

#define addSDFS(i) sdfs[zOrder[i]] = clamp(sdfs[zOrder[i]] - (1. - colorBlendModes[i]) * (sdfs[zOrder[i + 1]] + pSDFS[i]), 0, 1);
#expand addSDFS 19

    sdfs[zOrder[19]] -= (1. - colorBlendModes[19]) * sdfs[zOrder[20]];
    sdfs[zOrder[19]] = clamp(sdfs[zOrder[19]], 0, 1);
}