#version 450 core

layout (location = 0) in vec2 vTexCoord;

layout(location = 0) out vec4 vOutputColour;


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
}