#version 450 core

layout (location = 0) in vec2 vTexCoord;

layout(location = 0) out vec4 vOutputColour;

uniform sampler2D sampler0;

void main()
{
	// vOutputColour = vec4(vTexCoord,0,1);//texture(sampler0, vTexCoord);
	vOutputColour = texture(sampler0, vTexCoord);
}