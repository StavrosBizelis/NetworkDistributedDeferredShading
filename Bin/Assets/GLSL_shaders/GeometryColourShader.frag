#version 400 core

in vec3 vEyeNorm;			// Interpolated normal
in vec2 vTexCoord;			// Interpolated texture coordinate using texture coordinate from the vertex shader

uniform sampler2D u_diffuseMap;  // The texture sampler


uniform vec3 UDiffuse = vec3(1.);
uniform vec3 USpecular = vec3(1.);
uniform float UHardness = 0.5;
uniform vec3 UEmissive = vec3(0.);

layout(location = 0) out vec4 diffuseOut;
layout(location = 1) out vec4 normalOut;
layout(location = 2) out vec4 specularOut;
layout(location = 3) out vec4 emissiveOut;






void main()
{

    vec4 texDiffuse = texture(u_diffuseMap, vTexCoord);

    normalOut = vec4( normalize(vEyeNorm) , 1.); 
	  
    diffuseOut = vec4(UDiffuse,1) * texDiffuse;	// set texture color
    specularOut = vec4( USpecular, UHardness);
    emissiveOut = vec4(UEmissive, 1.);
}
