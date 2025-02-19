#version 330 core

// TODO 6-1:
// Implement Glass-Schlick shading

layout(location = 0) in vec3 aPosition;   // Vertex position
layout(location = 1) in vec3 aNormal;     // Vertex normal
layout(location = 2) in vec2 aTexCoord;   // Texture coordinates

out vec3 FragPos;     // World space position
out vec3 Normal;      // World space normal
out vec3 EyeDir;      // Direction to the camera (view vector)
out vec2 TexCoord;    // Texture coordinates

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 ViewPos; // Camera position

void main()
{
    FragPos = vec3(model * vec4(aPosition, 1.0f));  // Convert position to world space
    Normal = mat3(transpose(inverse(model))) * aNormal;  // Transform normal to world space
    EyeDir = normalize(FragPos-ViewPos);   // Direction to the camera
    TexCoord = aTexCoord;    // Pass texture coordinates to fragment shader
    gl_Position = projection * view * vec4(FragPos, 1.0);
}