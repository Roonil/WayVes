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

#ifndef TWOPI
#define TWOPI 6.28318530718
#endif
#ifndef PI
#define PI 3.14159265359
#endif
#define RAD_PI PI / 180.

#include ":$CONFIGFILE"

float glowLightVal(float glowValue, float glowLightStrengthValue, float glowLightDistanceValue)
{
    return glowLightDistanceValue + glowLightDistanceValue / pow(glowValue, glowLightStrengthValue);
}

vec4 addColors(float blendMode, vec4 above, vec4 below)
{
    return above + (1. - blendMode * above.w) * below;
}

#define initialiseGlowObject(I) Glow glow##I;
#expand initialiseGlowObject postProcessingNumber

void defaultGlowValues(inout Glow glow)
{
    glow.blendMode = 1;
    glow.mixAlpha = 1;
    glow.offsetAngle = 0;
    glow.size = 10;
    glow.intensity = .5;
    glow.directions = 8;

    glow.coords = gl_FragCoord.xy;

    glow.quality = 4;
    vec4 color = vec4(0.5, 0.5, 0.5, 1.0);
    glow.brightnessOffset = .0;
    glow.lightStrength = .5;
}

#define callSetGlow(I) setGlow##I(glow##I);

#define getGlow(I) glow = glow##I;

void main()
{

#define callDefaultGlowValues(I) defaultGlowValues(glow##I)
#expand callDefaultGlowValues postProcessingNumber

#expand callSetGlow postProcessingNumber

    Glow glow;

#expand getGlow postProcessingNumber

    vec2 uv = (glow.coords) / resolution.xy;
    vec4 prevColor = texture(tex, uv);

    vec2 glowRadius = (glow.size) / resolution.xy;
    vec4 Color = vec4(0);

    float glowOffsetValue = (float(glow.offsetAngle) / 360.) * TWOPI;

    for (float d = glowOffsetValue; d < TWOPI; d += TWOPI / (glow.directions)) {
        for (float i = 1.0 / (glow.quality); i <= 1.0; i += 1.0 / (glow.quality)) {
            vec2 coords = uv + glowRadius * i * vec2(cos(d), sin(d));

            if (coords.x > 0 && coords.x < 1 && coords.y > 0 && coords.y < 1)
                Color += texture(tex, coords);
        }
    }

    Color /= (glow.quality) * (glow.directions);

    FragColor = (vec4(glow.color.xyz * glow.color.w, glow.color.w)) * glow.intensity * length(Color);

    FragColor = addColors(glow.blendMode, prevColor, FragColor);
    FragColor *= glowLightVal(length(FragColor), glow.brightnessOffset, glow.lightStrength);

    FragColor.w = mix(prevColor.w, Color.w, glow.mixAlpha * 0.5);
}