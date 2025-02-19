#version 330 core

// TODO 6-2
// Implement Glass-Empricial shading

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D mytexture;
uniform samplerCube skybox;

uniform vec3 viewPos;
uniform float AIR_coeff;
uniform float GLASS_coeff;
uniform float Scale;
uniform float Power;
uniform float Bias;

void main()
{
    // Calculate reflection vector
    vec3 I = normalize(FragPos - viewPos);
    vec3 R = I - 2.0 * dot(I, normalize(Normal)) * normalize(Normal);
    
    // Calculate refraction vector
    float eta = AIR_coeff / GLASS_coeff;
    float cosThetaI = max(dot(-I, normalize(Normal)), 0.0);
    float sin2ThetaT = eta * eta * (1.0 - cosThetaI * cosThetaI);
    vec3 refractDir = eta * I + (eta * cosThetaI - sqrt(1.0 - sin2ThetaT)) * normalize(Normal);
    
    // Sample the environment color
    vec3 reflectColor = texture(skybox, R).rgb;
    vec3 refractColor = texture(skybox, refractDir).rgb;

    // Empirical approximation
    float cosTheta = max(dot(-I, normalize(Normal)), 0.0);
    float R_theta = max(0, min(1, Scale * pow(1.0 - cosTheta, Power) + Bias));

    // Mix the reflection and refraction colors
    vec3 finalColor = R_theta * reflectColor + (1 - R_theta) * refractColor;

    FragColor = vec4(finalColor, 1.0);
}