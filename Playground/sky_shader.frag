#version 330 core

in vec3 TexCoord; // Вектор направления, таже представляющий трехмерную текстурную координату
uniform samplerCube cubemap;
out vec4 color;

uniform vec3 brightness;

void main()
{
    color = texture(cubemap, TexCoord) * vec4(brightness, 1.0);
}
