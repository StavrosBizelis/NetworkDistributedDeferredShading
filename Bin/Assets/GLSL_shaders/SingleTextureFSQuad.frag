#version 400 core

in vec2 vTexCoord;			// Interpolated texture coordinate using texture coordinate from the vertex shader
uniform sampler2D UTexture;
out vec4 vOutputColour;


void main()
{
  vOutputColour = texture(UTexture, vTexCoord);
}
