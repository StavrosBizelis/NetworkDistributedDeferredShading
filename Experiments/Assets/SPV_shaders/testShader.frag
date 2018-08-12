#version 450 core

layout (location = 0) in vec2 vTexCoord;

layout(location = 0) out vec4 vOutputColour;
layout(location = 1) out vec4 vOutputColour2;
layout(location = 2) out vec4 vOutputColour3;
layout(location = 3) out vec4 vOutputColour4;


layout (std140, set = 0, binding = 2) uniform MaterialData
{
  vec4 UDiffuse;
  vec4 USpecular;
  vec4 UHardness;
  vec4 UEmissive;
} material;


layout(binding = 3) uniform sampler2D sampler0;

void main()
{
	vOutputColour = vec4(material.UEmissive.xyz + texture(sampler0, vTexCoord).xyz,1);
	// vOutputColour = vec4(vec3(1,1,0) ,1);
	vOutputColour2 = vec4(vec3(1,0,0) ,1);
	vOutputColour3 = vec4(vec3(0,0,1) ,1);
	vOutputColour4 = vec4(vec3(1,1,1) ,1);
}