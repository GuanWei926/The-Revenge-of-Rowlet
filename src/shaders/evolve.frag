#version 330 core

in vec2 GeoTexCoord; // Texture coordinates passed from the geometry shader

uniform sampler2D ourTexture; // Texture sampler
uniform vec3 rainbowColor;    // Rainbow color (could be used for effect modulation)
uniform float shimmer;           // Time for animation effect

out vec4 FragColor;

void main()
{
    // Sample the texture color
    vec4 textureColor = texture(ourTexture, GeoTexCoord);
    
    // Golden color: a bright yellowish color (golden effect)
    vec3 goldenColor = vec3(1.0, 0.843, 0.0); // Gold color (RGB)

    // White light shimmer (RGB: 1.0, 1.0, 1.0 for white light)
    vec3 whiteLight = vec3(1.0, 1.0, 1.0); // White light color

    // Apply the shimmer to the golden color
    vec3 finalColor = mix(goldenColor, whiteLight, shimmer);
    
    if (shimmer > 0) {
        finalColor = mix(finalColor, textureColor.rgb, 0.5); // Optionally, mix with texture color
    }
    else {
        finalColor = mix(finalColor, textureColor.rgb, 1.0);
    }

    // Set the final fragment color
    FragColor = vec4(finalColor, 1.0);  // Ensure full opacity
}