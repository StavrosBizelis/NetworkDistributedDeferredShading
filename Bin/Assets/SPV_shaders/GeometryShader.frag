#version 450 core

layout (location = 0) in vec2 vTexCoord;	  // Interpolated texture coordinate using texture coordinate from the vertex shader
layout (location = 1) in vec3 vEyeNorm;			// Interpolated normal
layout (location = 2) in vec3 vEyeTan;	    // tangent


layout (std140, set = 0, binding = 2) uniform MaterialData
{
  vec3 UDiffuse;
  vec3 USpecular;
  float UHardness;
  vec3 UEmissive;
} material;



layout(location = 0) out vec4 diffuseOut;
layout(location = 1) out vec4 normalOut;
layout(location = 2) out vec4 specularOut;
layout(location = 3) out vec4 emissiveOut;



void main()
{

    normalOut = vec4( normalize(vEyeNorm) , 1.); 
	  
    diffuseOut = vec4(material.UDiffuse, 1.);
    specularOut = vec4(material.USpecular, material.UHardness);
    emissiveOut = vec4(material.UEmissive, 1.);
}
