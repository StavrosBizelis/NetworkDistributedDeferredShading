#version 450 core

layout (location = 0) in vec2 vTexCoord;	  // Interpolated texture coordinate using texture coordinate from the vertex shader
layout (location = 1) in vec3 vEyeNorm;			// Interpolated normal
layout (location = 2) in vec3 vEyeTan;	    // tangent




layout (std140, set = 0, binding = 2) uniform MaterialData
{
  vec4 UDiffuse;
  vec4 USpecular;
  vec4 UHardness;
  vec4 UEmissive;
} material;

layout (set = 0, binding = 3) uniform sampler2D u_diffuseMap;  // The diffuse colour sampler

layout(location = 0) out vec4 diffuseOut;
layout(location = 1) out vec4 normalOut;
layout(location = 2) out vec4 specularOut;
layout(location = 3) out vec4 emissiveOut;






void main()
{

    vec4 texDiffuse = texture(u_diffuseMap, vTexCoord);

    normalOut = vec4( normalize(vEyeNorm) , 1.); 
	  
    diffuseOut = vec4(material.UDiffuse.xyz,1) * texDiffuse;	// set texture color
    specularOut = vec4( material.USpecular.xyz, material.UHardness.x);
    emissiveOut = vec4(material.UEmissive.xyz, 1.);
}
