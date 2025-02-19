#version 330 core

// TODO 3:
// Implement Gouraud shading

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 vertexColor;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float gloss;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main()
{
    // Transform the vertex position
    vec3 FragPos = vec3(model * vec4(aPos, 1.0));
    vec3 Normal = normalize(mat3(transpose(inverse(model))) * aNormal);

    // Ambient
    vec3 ambient = light.ambient * material.ambient;

    // Diffuse
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = 2.0 * dot(Normal, lightDir) * Normal - lightDir;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.gloss);
    vec3 specular = light.specular * (spec * material.specular);

    // Combine results
    vertexColor = ambient + diffuse + specular;

    TexCoords = aTexCoords;

    // Transform the vertex position to clip space
    gl_Position = projection * view * vec4(FragPos, 1.0);
}