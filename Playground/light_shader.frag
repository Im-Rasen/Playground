#version 330 core
#define NR_POINT_LIGHTS 4

out vec4 color;

in vec2 TexCoord;
in vec3 Normal;
in vec3 worldPosition; // Позиция текущего фрагмента
in vec4 dirShadowPosition; // Позиция текущего фрагмента в координатах источника света

uniform sampler2D shadowMap;

struct Material {
    sampler2D ambient;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
  
uniform Material material;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
};

uniform Light light[NR_POINT_LIGHTS];

struct DirectLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirectLight dirLight;

struct ProjectLight{
    vec3  position;
    vec3  direction;
    float cutOff;
    float outerCutOff;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
    
};

uniform ProjectLight projLight;

uniform vec3 viewPosition;


vec3 CalcDirLight(DirectLight dirLight, vec3 norm, vec3 viewDirection, float shadow);
vec3 CalcPointLight(Light light, vec3 norm, vec3 worldPosition, vec3 viewDirection);
vec3 CalcProjLight(ProjectLight projLight, vec3 norm, vec3 worldPosition, vec3 viewDirection);
float ShadowCalculation(vec4 dirShadowPosition);

void main()
{
    //"color = vertexColor;\n"
    //color = ourColor;
    //color = ourPosition;
    //color = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0f);
    //color = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), mixRate);
    //color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    
            //vec3 lightDir   = normalize(lightPos - FragPos);
            //vec3 halfwayDir = normalize(lightDir + viewDir);
    float shadow = ShadowCalculation(dirShadowPosition);
    
    vec3 norm = normalize(Normal);
    vec3 viewDirection = normalize(viewPosition - worldPosition);
    
    vec3 trueColor = vec3(0.0);
    trueColor += CalcDirLight(dirLight, norm, viewDirection, shadow);
    for (int i = 0; i < NR_POINT_LIGHTS; i++)
        trueColor += CalcPointLight(light[i], norm, worldPosition, viewDirection);
    trueColor += CalcProjLight(projLight, norm, worldPosition, viewDirection);
    

    vec4 theColor = vec4(trueColor, 1.0);
    color = theColor;
}

vec3 CalcDirLight(DirectLight dirLight, vec3 norm, vec3 viewDirection, float shadow)
{
    //Направленный свет
    //Ambient
    vec3 dirAmbient = dirLight.ambient * vec3(texture(material.ambient, TexCoord));
    //Diffuse
    vec3 dirLightDirection = normalize(-dirLight.direction); //В итоге - на источник от объекта
    vec3 halfwayDirection = normalize(dirLightDirection + viewDirection);//Медианный вектор
    float dirDiffuseRate = max(dot(norm, dirLightDirection), 0.0);
    vec3 dirDiffuse = dirLight.diffuse * dirDiffuseRate * vec3(texture(material.diffuse, TexCoord));
    //Specular
    vec3 dirReflectDirection = reflect(-dirLightDirection, norm);
    //float dirSpec = pow(max(dot(viewDirection, dirReflectDirection), 0.0), material.shininess);
    float dirSpec = pow(max(dot(norm, halfwayDirection), 0.0), material.shininess/2.0f);
    vec3 dirSpecular = dirLight.specular * dirSpec * vec3(texture(material.specular, TexCoord));
    return (dirAmbient + (1.0 - shadow) * dirDiffuse + (1.0 - shadow) * dirSpecular);
}


vec3 CalcPointLight(Light light, vec3 norm, vec3 worldPosition, vec3 viewDirection)
{
    //Лампа
    //Ambient
    vec3 ambient = light.ambient * vec3(texture(material.ambient, TexCoord));
    //Diffuse
    vec3 lightDirection = normalize(light.position - worldPosition); // Вектор от объекта к источнику
    vec3 halfwayDirection = normalize(lightDirection + viewDirection);//Медианный вектор
    float diffuseRate = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = light.diffuse * diffuseRate * vec3(texture(material.diffuse, TexCoord));
    //Specular
    vec3 reflectDirection = reflect(-lightDirection, norm); // Первый аргумент - от источника к объекту
    //float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
    float spec = pow(max(dot(norm, halfwayDirection), 0.0), material.shininess/2.0f);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
    //Attenuation
    float distance = length(light.position - worldPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));
    
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
    
    
}

vec3 CalcProjLight(ProjectLight projLight, vec3 norm, vec3 worldPosition, vec3 viewDirection)
{
    vec3 projDiffuse = vec3(0.0f);
    vec3 projSpecular = vec3(0.0f);
    vec3 projAmbient = projLight.ambient * vec3(texture(material.diffuse, TexCoord));
    
    vec3 projLightDirection = normalize(projLight.position - worldPosition);
    //Медианный вектор
    vec3 halfwayDirection = normalize(projLightDirection + viewDirection);
    
    float theta = dot(projLightDirection, normalize(-projLight.direction));
    float epsilon = projLight.cutOff - projLight.outerCutOff;
    float projIntensity = clamp((theta - projLight.outerCutOff) / epsilon, 0.0, 1.0);
    
    if(theta > projLight.outerCutOff) // cos
    {
        //Diffuse
        float projDiffuseRate = max(dot(norm, projLightDirection), 0.0);
        projDiffuse = projLight.diffuse * projDiffuseRate * vec3(texture(material.diffuse, TexCoord));
        //Specular
        vec3 projReflectDirection = reflect(-projLightDirection, norm);
        float projSpec = pow(max(dot(norm, halfwayDirection), 0.0), material.shininess/2.0f);
        projSpecular = projLight.specular * projSpec * vec3(texture(material.specular, TexCoord));
    }
    //Attenuation
    float projDistance = length(projLight.position - worldPosition);
    float projAttenuation = 1.0 / (projLight.constant + projLight.linear * projDistance +
    projLight.quadratic * (projDistance * projDistance));
    
    projDiffuse *= projAttenuation;
    projDiffuse *= projIntensity;
    projSpecular *= projAttenuation;
    projSpecular *= projIntensity;
    
    return (projAmbient + projDiffuse + projSpecular);
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = dirShadowPosition.xyz / dirShadowPosition.w;
    projCoords = projCoords * 0.5 + 0.5; //Координаты текстуры [0,1] <- координаты фрагментов [-1,1]
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
    return shadow;
}
