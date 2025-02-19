#version 330 core

// TODO 3:
// Implement Gouraud shading

in vec3 vertexColor;
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D mytexture;

void main()
{
    vec3 modelColor = texture(mytexture, TexCoords).rgb;
    FragColor = vec4(vertexColor * modelColor, 1.0);
}