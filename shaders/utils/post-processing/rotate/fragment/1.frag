#version 430 core

uniform int postProcessingNumber;
uniform vec2 resolution;

uniform sampler1D audioL;
uniform sampler1D audioR;

uniform float time;
uniform int audioLSize;
uniform int audioRSize;

uniform sampler2D tex;

out vec4 FragColor;

in vec2 r_resolution;
in vec4 r_gl_FragCoord;
in vec4 r_ngl_FragCoord;

#include ":$CONFIGFILE"

#ifndef TWOPI
#define TWOPI 6.28318530718
#endif
#ifndef PI
#define PI 3.14159265359
#endif
#define RAD_PI PI / 180.

#define initialiseRotateObject(I) Rotate rotate##I;
#expand initialiseRotateObject postProcessingNumber

void defaultRotateValues(inout Rotate rotate)
{
    rotate.transform = IDENTITY_MATRIX;
    rotate.center = vec2(resolution) / 2.;
    rotate.angle = 0;
    rotate.coords = gl_FragCoord.xy;
}

#define callSetRotate(I) setRotate##I(rotate##I);

#define getRotate(I) rotate = rotate##I;

void main()
{

#define callDefaultRotateValues(I) defaultRotateValues(rotate##I)
#expand callDefaultRotateValues postProcessingNumber

#expand callSetRotate postProcessingNumber

    Rotate rotate;

#expand getRotate postProcessingNumber

    float angle = rotate.angle * RAD_PI;
    float angleSin = sin(angle), angleCos = cos(angle);

    vec2 uv = (rotate.transform * mat2(vec2(angleCos, -angleSin), vec2(angleSin, angleCos)) * (rotate.coords - rotate.center) + rotate.center) / resolution.xy;
    FragColor = texture(tex, uv);
}