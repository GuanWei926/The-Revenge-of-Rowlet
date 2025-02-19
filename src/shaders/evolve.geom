#version 330 core

layout (triangles) in;
layout (triangle_strip,max_vertices=3) out;

in VS_OUT{
    vec2 TexCoords;
}gs_in[]; // Interpolated texture coordinates from vertex shader

out vec2 GeoTexCoord; // Texture coordinates for fragment shader
uniform float time;

vec4 explode(vec4 position, vec3 normal){
    float magnitude=100.0;
    vec3 direction=normal*time*magnitude;
    return position + vec4(direction,0.0);
}

vec3 GetNormal(){
    vec3 a=vec3(gl_in[0].gl_Position)-vec3(gl_in[1].gl_Position);
    vec3 b=vec3(gl_in[2].gl_Position)-vec3(gl_in[1].gl_Position);
    return normalize(cross(a,b));
}

void main()
{
    vec3 normal=GetNormal();
    for (int i = 0; i < 3; i++) // Iterate over the 3 vertices of the triangle
    {
        gl_Position = explode(gl_in[i].gl_Position,normal); // Pass through the vertex position
        GeoTexCoord = gs_in[i].TexCoords;        // Pass through the texture coordinate
        EmitVertex();                      // Emit the vertex
    }
    EndPrimitive(); // Ends the current primitive
}