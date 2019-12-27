#version 330 core
#define NR_POINT_LIGHTS 4

out vec4 finalColor;

in vec2 TexCoord;
//in vec3 Normal;
in vec3 worldPosition;
in vec3 tangentViewPosition;
in vec3 tangentPosition;
in vec3 tangentLightPosition;//[NR_POINT_LIGHTS];

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform float heightScale;

vec2 ParallaxMapping(vec2 texCoord, vec3 viewDirection){
    float height =  texture(depthMap, texCoord).r;
    return texCoord - viewDirection.xy * (height * heightScale);
}
vec2 ReliefPMapping(vec2 texCoord, vec3 viewDirection){
    const float minLayers = 16.0;
    const float maxLayers = 64.0;
    // количество слоев глубины
    float nLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDirection)));
    // размер каждого слоя
    float layerDepth = 1.0 / nLayers;
    // глубина текущего слоя
    float currentLayerDepth = 0.0;
    // величина шага смещения текстурных координат на каждом слое
    // расчитывается на основе вектора P
    vec2 P = (viewDirection.xy/viewDirection.z) * heightScale;
    vec2 deltaTexCoord = P / nLayers;
    // начальная инициализация
    vec2  currentTexCoord      = texCoord;
    float currentDepthMapValue = texture(depthMap, currentTexCoord).r;
    
    while(currentLayerDepth < currentDepthMapValue)
    {
        // смещаем текстурные координаты вдоль вектора P
        currentTexCoord -= deltaTexCoord;
        // делаем выборку из карты глубин в текущих текстурных координатах
        currentDepthMapValue = texture(depthMap, currentTexCoord).r;
        // рассчитываем глубину следующего слоя
        currentLayerDepth += layerDepth;
    }

    //уполовинить смещение текстурных координат и размер слоя глубины
    deltaTexCoord *= 0.5;
    layerDepth *= 0.5;
    // сместимся в обратном направлении от точки, найденной в Steep PM
    currentTexCoord += deltaTexCoord;
    currentLayerDepth -= layerDepth;
    // установим максимум итераций поиска…
    const int RELIEF_STEPS = 10;
    int currentStep = RELIEF_STEPS;
    while (currentStep > 0) {
        currentDepthMapValue = texture(depthMap, currentTexCoord).r;
        deltaTexCoord *= 0.5;
        layerDepth *= 0.5;
            // если выборка глубины больше текущей глубины слоя,
            // то уходим в левую половину интервала
            if (currentDepthMapValue > currentLayerDepth) {
                currentTexCoord -= deltaTexCoord;
                currentLayerDepth += layerDepth;
            }
            // иначе уходим в правую половину интервала
            else {
                currentTexCoord += deltaTexCoord;
                currentLayerDepth -= layerDepth;
            }
            currentStep--;
        }
        
    return currentTexCoord;
    //-------------------------------------------------------------------
    /*
    float height =  texture(depthMap, texCoord).r;
    vec2 p = viewDirection.xy / viewDirection.z * (height * heightScale);
    return texCoord - p;
    */
}


void main()
{
    // offset texture coordinates with Parallax Mapping
    vec3 viewDirection = normalize(tangentViewPosition - tangentPosition);
    vec2 texCoord = TexCoord;
    
    //texCoord = ParallaxMapping(texCoord,  viewDirection);
    texCoord = ReliefPMapping(texCoord,  viewDirection);
    if(texCoord.x > 1.0 || texCoord.y > 1.0 || texCoord.x < 0.0 || texCoord.y < 0.0)
        discard;

    // obtain normal from normal map
    vec3 norm = texture(normalMap, texCoord).rgb;
    norm = normalize(norm * 2.0 - 1.0);
   
    // get diffuse color
    vec3 color = texture(diffuseMap, texCoord).rgb;
    vec3 trueColor = vec3(0.0);
    // ambient
    vec3 ambient = 0.1 * color;
    /*
    trueColor = ambient;
    for (int i = 0; i < NR_POINT_LIGHTS; i++){
        // diffuse
        vec3 lightDirection = normalize(tangentLightPosition[i] - tangentPosition);
        float diff = max(dot(lightDirection, norm), 0.0);
        vec3 diffuse = diff * color;
        // specular
        vec3 reflectDirection = reflect(-lightDirection, norm);
        vec3 halfwayDirection = normalize(lightDirection + viewDirection);
        float spec = pow(max(dot(norm, halfwayDirection), 0.0), 32.0);
        vec3 specular = vec3(0.2) * spec;
        trueColor += diffuse + specular;
    }
     */
    // diffuse
    vec3 lightDirection = normalize(tangentLightPosition - tangentPosition);
    float diff = max(dot(lightDirection, norm), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 reflectDirection = reflect(-lightDirection, norm);
    vec3 halfwayDirection = normalize(lightDirection + viewDirection);
    float spec = pow(max(dot(norm, halfwayDirection), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec;
    finalColor = vec4(ambient + diffuse + specular, 1.0);
    
    //Color = vec4(trueColor, 1.0);
}
