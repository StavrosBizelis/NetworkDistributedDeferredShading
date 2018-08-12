#version 400 core

in vec2 vTexCoord;			// Interpolated texture coordinate using texture coordinate from the vertex shader
uniform sampler2D UTexture;
uniform sampler2D UBloomTexture0;
uniform float UGamma = 1;
uniform float UExposure = 1;

out vec4 vOutputColour;

// algorithm based on the tutorial at: https://learnopengl.com/Advanced-Lighting/HDR
void main()
{
	vec3 l_color = vec3(0);
    l_color += texture(UTexture, vTexCoord).xyz;
	l_color += texture(UBloomTexture0, vTexCoord).xyz;

    // exposure tone mapping
    vec3 l_mappedColor = vec3(1.0) - exp(-l_color * UExposure);
    
    // gamma correction 
    l_mappedColor = pow(l_mappedColor, vec3(1.0 / UGamma));
    
    vOutputColour = vec4(l_mappedColor, 1.0);
}
