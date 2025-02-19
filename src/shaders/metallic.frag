#version 330 core

// TODO 5:
// Implement Metallic shading

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D mytexture;
uniform samplerCube skybox;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform float bias;
uniform float alpha;
uniform float lightIntensity;

void main()
{
    // Sample the model texture color
    vec3 modelColor = texture(mytexture, TexCoords).rgb;

    // Calculate reflection vector
    vec3 I = normalize(FragPos - viewPos);
    vec3 R = I - 2.0 * dot(I, normalize(Normal)) * normalize(Normal);
    
    // Sample the environment color
    vec3 reflectColor = texture(skybox, R).rgb;

    // Lambertian reflection
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(normalize(Normal), lightDir), 0.0);
    vec3 B = vec3(lightIntensity * diff + bias);

    // Mix the model texture color with the reflected color
    vec3 finalColor = (alpha + 0.1) * 1.3 * B * modelColor + (1.0 - (alpha + 0.1)) * 1.3 * reflectColor;

    FragColor = vec4(finalColor, 1.0);
}
