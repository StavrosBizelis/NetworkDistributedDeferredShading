#version 400 core

in vec3 vEyeNorm;			// Interpolated normal


uniform vec3 UDiffuse = vec3(1);
uniform vec3 USpecular = vec3(1.);
uniform float UHardness = 0.5;
uniform vec3 UEmissive = vec3(0.);


layout(location = 0) out vec4 diffuseOut;
layout(location = 1) out vec4 normalOut;
layout(location = 2) out vec4 specularOut;
layout(location = 3) out vec4 emissiveOut;



void main()
{

    normalOut = vec4( normalize(vEyeNorm) , 1.); 
	  
    diffuseOut = vec4(UDiffuse, 1.);
    specularOut = vec4(USpecular, UHardness);
    emissiveOut = vec4(UEmissive, 1.);
}
