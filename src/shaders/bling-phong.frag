#version 330 core

// TODO 2
// Implement Bling-Phong shading

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

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D mytexture;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    vec3 modelColor = texture(mytexture, TexCoords).rgb;

    // Ambient
    vec3 ambient = light.ambient * material.ambient;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.gloss);
    vec3 specular = light.specular * (spec * material.specular);
    
    vec3 result = (ambient + diffuse + specular) * modelColor;
    FragColor = vec4(result, 1.0);
}