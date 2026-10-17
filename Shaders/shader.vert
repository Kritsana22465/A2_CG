#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 aNormal;

out vec4 VCol;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 lightProjection;
uniform mat4 lightView;

void main()
{
    vec4 worldPosition = model * vec4(pos, 1.0);
    gl_Position = projection * view * worldPosition;
    FragPos = worldPosition.xyz;
    VCol = vec4(clamp(pos, 0.0, 1.0), 1.0);
    TexCoord = texCoord;
    Normal = mat3(transpose(inverse(model))) * aNormal;
    FragPosLightSpace = lightProjection * lightView * worldPosition;
}