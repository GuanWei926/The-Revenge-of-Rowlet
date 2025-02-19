#version 330 core

// TODO 4-1
// Implement CubeMap shading

layout(location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aPos;
    
    // Remove translation part of the view matrix
    mat4 viewNoTranslation = mat4(mat3(view));
    vec4 pos = projection * viewNoTranslation * vec4(aPos, 1.0);
    // Set w component to z component to push the cube to infinity
    gl_Position = pos.xyww; 
}