#version 330 core
out vec4 colour;

in vec4 fragPosLightSpace;

uniform vec3 bgColour;
uniform sampler2D shadowMap;

float ShadowCalculation()
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // ถ้าพิกัดหลุดออกนอกกล้องของแสง ไม่ให้เกิดเงา (แก้ปัญหาเงาลากยาวติดขอบ)
    if (projCoords.z > 1.0 || projCoords.z < 0.0 ||
        projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;
    float bias = 0.005;
    float shadow = 0.0;

    // PCF Filtering (Smoothing เงา)
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

void main()
{
    float shadow = ShadowCalculation();
    colour = vec4((1.0 - shadow) * bgColour, 1.0f);
}