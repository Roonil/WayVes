#version 430 core
uniform sampler1D audioR;

out vec4 fragment;
in vec4 gl_FragCoord;

/* 1D texture mapping */
void main() {
    
    fragment.r = texelFetch(audioR, int(gl_FragCoord.x), 0).r;
}
