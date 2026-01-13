layout(location = 0) in vec3 aPos;

precision highp float;
out vec4 r_ngl_FragCoord;
out vec2 r_resolution;

uniform vec2 resolution;

uniform sampler1D audioL;
uniform sampler1D audioR;

uniform float time;
uniform int audioLSize;
uniform int audioRSize;

#define TWOPI 6.28318530718
#define PI 3.14159265359

#include ":$CONFIGFILE"

mat2 ApplyResolutionRotation(float angle)
{
    return mat2(vec2(cos(angle), sin(angle)), vec2(sin(angle), cos(angle)));
}

mat2 ApplyRotation(float angle)
{
    return mat2(vec2(cos(angle), -sin(angle)), vec2(sin(angle), cos(angle)));
}
void main()
{
    float angle = (coordinateRotation)*PI / 180.;
    float resAngle = acos(cos(angle));
    resAngle = resAngle > PI / 2. ? PI - resAngle : resAngle;

    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);

    r_ngl_FragCoord.xy = (ApplyRotation(angle) * (gl_Position.xy));

    r_resolution.x = 1.;
    r_resolution.y = resolution.y / resolution.x;
    r_resolution = ApplyResolutionRotation(resAngle) * r_resolution;

    r_resolution *= resolution.x / (sin(resAngle) + cos(resAngle));

    r_ngl_FragCoord.xy += 1.;
    r_ngl_FragCoord.xy /= 2.;
}