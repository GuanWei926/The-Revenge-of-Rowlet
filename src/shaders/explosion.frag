#version 330 core
out vec4 FragColor;

in vec2 TexCoords; 

uniform sampler2D ourTexture;
uniform vec3 rainbowColor;

void main()
{
    FragColor = texture(ourTexture, TexCoords);
} 