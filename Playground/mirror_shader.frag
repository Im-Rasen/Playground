#version 330 core

out vec4 color;

in vec3 TexCoord; // Вектор направления, таже представляющий трехмерную текстурную координату
in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPosition;
uniform samplerCube cubemap;
uniform vec3 sunBright;
//uniform vec3 lampBright;


void main()
{
    //color = texture(cubemap, TexCoord);
    vec3 I = normalize(Position - cameraPosition);
    vec3 R = reflect(I, normalize(Normal));
    vec3 brightness = sunBright;// + lampBright;
    color = vec4(texture(cubemap, R).rgb, 1.0) * vec4(brightness, 1.0);
    //color = vec4(texture(cubemap, I).rgb, 1.0) * vec4(brightness, 1.0);
}
