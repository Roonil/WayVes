// Adapted from GLava's Gravity Shader by jarcode-foss
// Licensed under GPL-3.0

#version 330 core

uniform sampler1D audioR;
uniform float diff;

out vec4 FragColor;
in vec4 gl_FragCoord;

void main()
{

    FragColor.r = texelFetch(audioR, int(gl_FragCoord.x), 0).r - diff;
}
