#version 330 core
layout (location = 0) in vec3 position; // Позиция - с координатами в 0
//layout (location = 1) in vec3 color; // Цвет - с координатами в 1
//layout (location = 2) in vec2 texCoord; // Текстура - с координатами в 2

//out vec4 ourPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float shiftX;
uniform float shiftZ;

void main()
{
    gl_Position = projection * view * model * vec4(position[0], position[1], position[2], 1.0f);
}
