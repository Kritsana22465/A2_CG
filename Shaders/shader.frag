#version 330 core

in vec4 VCol;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform vec3 lightColour;
uniform sampler2D texture1;
uniform sampler2D shadowMap;
uniform bool useTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;

float calculateShadow(vec3 normal, vec3 lightDirection)
{
    vec3 projected = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projected = projected * 0.5 + 0.5;
    if (projected.z > 1.0 || projected.z < 0.0 ||
        projected.x < 0.0 || projected.x > 1.0 ||
        projected.y < 0.0 || projected.y > 1.0)
        return 0.0;

    float bias = max(0.0015 * (1.0 - dot(normal, lightDirection)), 0.0003);
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    float shadow = 0.0;
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float closestDepth = texture(shadowMap, projected.xy + vec2(x, y) * texelSize).r;
            shadow += projected.z - bias > closestDepth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

vec3 ambientLight()
{
    float ambientStrength = 0.2f;
    return lightColour * ambientStrength;
}

vec3 diffuseLight()
{
    float diffuseStrength = 0.5f;
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = lightColour * diff * diffuseStrength;
    return diffuse;
}

vec3 specularLight()
{
    float specularStrength = 0.8f;
    float shininess = 64.0f;
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(0.0f, dot(viewDir, reflectDir)), shininess);
    //vec3 halfDir = normalize(lightDir + viewDir);
    //float spec = pow(max(dot(norm, halfDir), 0.0f), shininess);
    vec3 specular = lightColour * spec * specularStrength;
    return specular;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float shadow = calculateShadow(norm, lightDir);
    vec3 ambient = ambientLight() * 0.7;
    vec3 diffuse = diffuseLight() * (1.0 - shadow);
    vec3 specular = specularLight() * (1.0 - shadow);
    vec3 resultLighting = ambient + diffuse + specular;
    vec4 baseColor = useTexture ? texture(texture1, TexCoord) : vec4(1.0f);
    FragColor = baseColor * vec4(resultLighting, 1.0f);
}