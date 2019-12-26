#version 330 core

layout (location = 0) in vec3 position; // Позиция - с координатами в 0
layout (location = 1) in vec2 texCoord; // Текстура - с координатами в 1
//layout (location = 3) in vec3 normal;

out vec2 TexCoord;
//out vec3 Normal;
//out vec3 worldPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//uniform float shiftX;
//uniform float shiftY;
//uniform float shiftZ;

void main()
{
    //vec4 some = model * vec4(position, 1.0);
    //some = vec4(some[0] + shiftX, some[1] + shiftY, some[2] + shiftZ, 1.0f);
    //gl_Position = projection * view * some; //+shiftX
    //gl_Position = projection * view * model * vec4(position[0] + shiftX, position[1] + shiftY, position[2] + shiftZ, 1.0);
    vec3 positionFollow = inverse(mat3(view))*position;
    gl_Position = projection * view * model * vec4(positionFollow, 1.0);
    //worldPosition = vec3(model * vec4(position, 1.0f));
    //Normal = mat3(transpose(inverse(model))) * normal;
    TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
}
