#version 330 core
layout(location = 0) in vec3 aPos;

out vec2 r_resolution;
out vec4 r_gl_FragCoord;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0);
}