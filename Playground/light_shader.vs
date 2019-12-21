#version 330 core

layout (location = 0) in vec3 position; // Позиция - с координатами в 0
//layout (location = 1) in vec3 color; // Цвет - с координатами в 1
layout (location = 1) in vec2 texCoord; // Текстура - с координатами в 2
layout (location = 2) in vec3 normal; // Нормаль - с координатами в 3

//"out vec4 vertexColor;" // Передача цвета во фрагментный шейдер
//out vec3 ourColor;
//out vec3 Position;
out vec2 TexCoord;
out vec3 Normal;
out vec3 worldPosition;
out vec4 dirShadowPosition;

uniform float shiftX;
uniform float shiftY;
uniform float shiftZ;
//uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 dirShadowMatrix;

void main()
{
    gl_Position = projection * view * model *vec4(position[0] + shiftX, position[1] + shiftY, position[2] + shiftZ, 1.0);
    //Normal = normal;
    worldPosition = vec3(model * vec4(position, 1.0f));
    Normal = mat3(transpose(inverse(model))) * normal;
    //ourColor = color; // Значение цвета от вершинных данных
    //"vertexColor = vec4(0.5f, 0.0f, 0.0f, 1.0f);"
    TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
    dirShadowPosition = dirShadowMatrix * vec4(worldPosition, 1.0);
}
