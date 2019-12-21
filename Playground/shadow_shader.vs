#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 dirShadowMatrix;
uniform mat4 model;

void main()
{
    gl_Position = dirShadowMatrix * model * vec4(position, 1.0);
}
