#version 330 core

// TODO 6-1:
// Implement Glass-Schlick shading

in vec3 FragPos;       // World space position
in vec3 Normal;        // World space normal
in vec3 EyeDir;        // Direction to the camera
in vec2 TexCoord;      // Texture coordinates

out vec4 FragColor;

uniform sampler2D ourTexture;   // Model texture
uniform samplerCube envMap;     // Environment map for reflection and refraction
uniform vec3 lightPos;          // Light position
uniform vec3 lightColor;        // Light color
uniform vec3 ViewPos;           // Camera position

// Hyperparameters
const float AIR_coeff = 1.0;  // Refractive index for air
const float GLASS_coeff = 1.52;  // Refractive index for glass
const float bias = 0.2;       // Bias for diffuse
const float alpha = 0.4;      // Mixing factor for reflection and refraction

void main()
{
    vec3 norm = normalize(Normal);     // Normalize normal
    vec3 lightDir = normalize(lightPos - FragPos);  // Direction from fragment to light
    vec3 viewDir = normalize(FragPos-ViewPos);    // Direction to the camera (view vector)
    
    // Calculate reflect_coef (ratio of refractive indices)
    float reflect_coef = AIR_coeff / GLASS_coeff;

    // Calculate the dot product between view direction and normal
    float dotProduct = dot(viewDir, norm);

    // Calculate R0 (Schlick approximation)
    float R0 = pow((AIR_coeff - GLASS_coeff) / (AIR_coeff + GLASS_coeff), 2.0);

    // Schlick approximation for the reflection coefficient R_theta
    float R_theta = R0 + (1.0 - R0) * pow(1.0 + dotProduct, 5.0);

    // Calculate the refraction direction (T)
    vec3 refractDir;
    float k = 1.0 - reflect_coef * reflect_coef * (1.0 - dotProduct * dotProduct);

    if (k < 0.0)
    {
        refractDir = vec3(0.0);  // Total internal reflection
    }
    else
    {
        refractDir = reflect_coef*viewDir - (reflect_coef * dot(viewDir, norm)+ sqrt(k)) * norm; // Refract formula
    }

    vec3 reflectDir = viewDir - 2.0 * dot(viewDir, norm) * norm;

    // Sample reflection and refraction colors
    vec3 reflectColor = texture(envMap, reflectDir).rgb;
    vec3 refractColor = texture(envMap, refractDir).rgb;

    // Mix reflection and refraction using Schlick approximation
    vec3 finalColor = R_theta * reflectColor + (1.0 - R_theta) * refractColor;

    FragColor = vec4(finalColor, 1.0);  // Final output color
}