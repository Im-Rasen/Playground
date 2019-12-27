
#version 330 core
#define NR_POINT_LIGHTS 4

layout (location = 0) in vec3 position; // Позиция - с координатами в 0
//layout (location = 1) in vec3 color; // Цвет - с координатами в 1
layout (location = 1) in vec2 texCoord; // Текстура - с координатами в 1
layout (location = 2) in vec3 normal; // Нормаль - с координатами в 2
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

//"out vec4 vertexColor;" // Передача цвета во фрагментный шейдер
//out vec3 ourColor;
//out vec3 Position;

out vec2 TexCoord;
//out vec3 Normal;
out vec3 worldPosition;
out vec3 tangentViewPosition;
out vec3 tangentPosition;
out vec3 tangentLightPosition;//[NR_POINT_LIGHTS];
//out mat3 TBN;

uniform float shiftX;
uniform float shiftY;
uniform float shiftZ;
//uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPosition;
uniform vec3 lightPosition;//[NR_POINT_LIGHTS];

void main()
{
       //Normal = normal;
    worldPosition = vec3(model * vec4(position, 1.0f));
    TexCoord = vec2(texCoord.x, texCoord.y);
    
    //Normal = mat3(transpose(inverse(model))) * normal;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * tangent);
    vec3 N = normalize(normalMatrix * normal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N));
    /*
    for (int i = 0; i < NR_POINT_LIGHTS; i++)
        tangentLightPosition[i] = TBN * lightPosition[i];
    */
    tangentLightPosition = TBN * lightPosition;
    tangentViewPosition  = TBN * viewPosition;
    tangentPosition      = TBN * worldPosition;
    //Таблица TBN
    //vec3 T = normalize(vec3(model * vec4(tangent,   0.0)));
    //vec3 B = normalize(vec3(model * vec4(bitangent, 0.0)));
    //vec3 N = normalize(vec3(model * vec4(normal,    0.0)));
    //TBN = mat3(T, B, N);
    gl_Position = projection * view * model * vec4(position[0] + shiftX, position[1] + shiftY, position[2] + shiftZ, 1.0);
}
